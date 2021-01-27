
import argparse, subprocess, shutil, os

def get_current_mutant_name(mutants, plugin_name, filename):
    with open("killed.txt") as file:
        mutant_index = len(file.readlines())
    with open("notkilled.txt") as file:
        mutant_index = mutant_index + len(file.readlines())

    # mutant sources have the format
    # <filename>.mutant.<id>.(h|cpp)
    current_mutant = mutants[mutant_index]
    filename_base = filename.split("/")[-1].split(".")[0]
    mutant_id = current_mutant.split(".")[2]
    return plugin_name + "_" + filename_base + "_" + mutant_id


parser = argparse.ArgumentParser("compile-mutant")
parser.add_argument("input_folder", help="Path to folder containing mutant sources")
parser.add_argument("output_folder", help="Path to output folder for mutant")
parser.add_argument("plugin_name", help="The pluginname, e.g. frequalizer)")
parser.add_argument("filename", help="original filename")

args = parser.parse_args()

# move plugin to output folder
mutants = os.listdir(args.input_folder)
mutant_plugin_name = get_current_mutant_name(mutants, args.plugin_name, args.filename)
if mutant_plugin_name + ".vst3" not in os.listdir(args.output_folder):
    subprocess.run(f"make {args.plugin_name}_VST3".split(" "))
    shutil.copytree(f"ext/{args.plugin_name}_artefacts/Release/VST3/{args.plugin_name}.vst3", f"{args.output_folder}/{mutant_plugin_name}.vst3")




