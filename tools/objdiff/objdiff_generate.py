# Adapted from https://github.com/Vatuu/silent-hill-decomp/tree/master

from pathlib import Path
from argparse import ArgumentParser
from dataclasses import dataclass, asdict
import logging
import yaml
import json

@dataclass
class UnitMetadata:
    progress_categories: list[str]

@dataclass
class Scratch:
    platform: str
    compiler: str
    c_flags: str
    ctx_path: str
    build_ctx: bool

@dataclass
class Unit:
    name: str
    base_path: str
    target_path: str
    metadata: UnitMetadata
    scratch: Scratch = None

@dataclass
class ProgressCategory:
    id: str
    name: str

@dataclass
class SSHConfig:
    """SSH configuration for remote builds"""
    enabled: bool = False
    host: str = ""
    user: str = ""
    port: int = 22
    remote_path: str = ""
    identity_file: str = ""

@dataclass
class Config:
    build_base: bool
    build_target: bool
    custom_make: str
    custom_args: list[str]
    units: list[Unit]
    progress_categories: list[ProgressCategory]


def _create_config():
    parser = ArgumentParser()
    parser.add_argument("config", type = Path)
    args = parser.parse_args()

    if not args.config.exists() or args.config.is_dir() or args.config.suffix != ".yaml":
        raise ValueError(f"The given path {args.objects} is not pointing towards a valid config.")

    with open(args.config) as stream:
        try:
            return yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            raise exc

EXCLUDED_NAMES = {"data", "rodata", "sdata", "bss"}

def _collect_objects(path: Path, config) -> list[Path]:
    ignored = config["ignored_files"]
    return [
        path for path in path.rglob("*.o")
        if not any(name in path.name for name in EXCLUDED_NAMES ) and not any(file in str(path) for file in ignored)
    ]

def _determine_categories(path: Path, config) -> tuple[UnitMetadata, str]:
    if path.name.endswith(".s.o"):
        modified_path = path.relative_to(config["expected_paths"]["asm"])
    else:
        modified_path = path.relative_to(config["expected_paths"]["src"])

    categories = []
    for category in config["categories"]:
        for prefix in category["paths"]:
            if str(modified_path).startswith(prefix):
                categories.append(category["id"])
    return (UnitMetadata(categories), str(modified_path))

def _parse_ssh_config(config: dict) -> SSHConfig:
    """Parse SSH configuration from YAML"""
    ssh_data = config.get("ssh", {})
    return SSHConfig(
        enabled=ssh_data.get("enabled", False),
        host=ssh_data.get("host", ""),
        user=ssh_data.get("user", ""),
        port=ssh_data.get("port", 22),
        remote_path=ssh_data.get("remote_path", ""),
        identity_file=ssh_data.get("identity_file", "")
    )

def _build_custom_make(ssh_config: SSHConfig) -> tuple[str, list[str]]:
    """Build custom_make and custom_args for objdiff"""
    if not ssh_config.enabled:
        return "make", ["progress"]
    
    # Build SSH arguments
    ssh_args = []
    
    # Add SSH options if needed
    if ssh_config.port != 22:
        ssh_args.extend(["-p", str(ssh_config.port)])
    if ssh_config.identity_file:
        import os
        identity_file = os.path.expanduser(ssh_config.identity_file) if ssh_config.identity_file.startswith("~/") else ssh_config.identity_file
        ssh_args.extend(["-i", identity_file])
    
    # Add host
    host_string = f"{ssh_config.user}@{ssh_config.host}" if ssh_config.user else ssh_config.host
    ssh_args.append(host_string)
    
    # Add remote commands
    ssh_args.append(f"cd {ssh_config.remote_path} &&")
    
    ssh_args.append("make progress")
    
    return "ssh", ssh_args

def main():
    logging.basicConfig(level = logging.INFO)
    config = _create_config()
    
    # Parse SSH configuration
    ssh_config = _parse_ssh_config(config)
    
    expected_objects = _collect_objects(Path(config["expected_paths"]["asm"]), config) + _collect_objects(Path(config["expected_paths"]["src"]), config)
    
    logging.info(f"Accounting for {len(expected_objects)} objects.")
    if ssh_config.enabled:
        logging.info(f"SSH Mode enabled: {ssh_config.user}@{ssh_config.host}:{ssh_config.remote_path}")
    else:
        logging.info(f"Local build mode")

    scratch_config = config.get("scratch", {})
    enable_scratch = scratch_config.get("enabled", False)

    units = []
    for file in expected_objects:
        processed_path = _determine_categories(file, config)
        base_path = "build/src/" + processed_path[1].removesuffix(".s.o").removesuffix(".c.o") + ".c.o"

        scratch = None
        if enable_scratch:
            scratch = Scratch(
                platform=scratch_config.get("platform", "ps1"),
                compiler=scratch_config.get("compiler", "gcc2.8.1-psx"),
                c_flags=scratch_config.get("c_flags", ""),
                ctx_path=scratch_config.get("ctx_path", "ctx/project.ctx.c"),
                build_ctx=scratch_config.get("build_ctx", True)
            )

        unit = Unit(
            processed_path[1].removesuffix(".s.o").removesuffix(".c.o"),
            base_path if Path(base_path).exists() else None,
            str(file),
            processed_path[0],
            scratch
        )
        units.append(unit)

    categories = []
    for category in config["categories"]:
        categories.append(ProgressCategory(category["id"], category["name"]))
    
    custom_make, custom_args = _build_custom_make(ssh_config)
    
    with (Path(config["output"])).open("w") as json_file:
        # Create config with SSH-aware custom_make
        objdiff_config = Config(
            build_base=True,
            build_target=False,
            custom_make=custom_make,
            custom_args=custom_args,
            units=units,
            progress_categories=categories
        )
        
        json.dump(asdict(objdiff_config), json_file, indent=2)
    
    logging.info(f"Generated {config['output']}")
    if ssh_config.enabled:
        logging.info(f"  custom_make: {custom_make}")

if __name__ == "__main__":
    import sys
    sys.exit(main())
