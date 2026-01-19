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

def main():
    logging.basicConfig(level = logging.INFO)
    config = _create_config()

    expected_objects = _collect_objects(Path(config["expected_paths"]["asm"]), config) + _collect_objects(Path(config["expected_paths"]["src"]), config)
    
    logging.info(f"Accounting for {len(expected_objects)} objects.")

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
    
    with (Path(config["output"])).open("w") as json_file:
        # Custom dict conversion to handle None values properly
        config_dict = asdict(Config(True, False, "make", ["progress"], units, categories))

        # Remove None scratch entries to keep JSON clean
        # if not enable_scratch:
        #     for unit in config_dict["units"]:
        #         if unit["scratch"] is None:
        #             del unit["scratch"]

        json.dump(asdict(Config(True, False, "make", ["progress"], units, categories)), json_file, indent=2)

if __name__ == "__main__":
    main()