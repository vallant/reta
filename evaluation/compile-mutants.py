import argparse, subprocess, os, sys

parser = argparse.ArgumentParser("compile-mutants")
parser.add_argument("base_folder", help="Path to folder containing individual mutant file folders")
parser.add_argument("plugin_name", help="The plugin-name as it appears in the binary")
parser.add_argument("working_directory", help="Absolute path to working directory where the compile command should be executed (relative to current folder)")
parser.add_argument("compile_command", help="Command used to compile the plugin")
parser.add_argument("plugin_output_path", help="Absolute path where the compiled plugin is located, including extension (e.g. bin/foo.vst3)")


args = parser.parse_args()

for mutant_folder in os.listdir(args.base_folder):

    # the plugins subfolder doesn't contain any sources
    if "plugins" in mutant_folder:
        continue

    # the mutant_folder must have (at least!) the structure:
    # folder/
    # - chosen_src/
    # - plugins/
    # - original_file.txt

    mutant_folder = args.base_folder + "/" + mutant_folder
    mutant_sources = mutant_folder + "/chosen_src"

    # the original_file.txt is expected to contain the absolute path to the original file
    with open(mutant_folder + "/original_file.txt") as f:    
        original_file = f.readline().strip("\n")

    # the compile script is expected to be a sibling file
    evaluation_folder = os.path.dirname(os.path.realpath(__file__))    

    # sys.executable is the current python interpreter
    compile_command = f"{sys.executable} {evaluation_folder}/compile-mutant.py {args.plugin_name} {args.working_directory} {mutant_sources} {args.compile_command} {args.plugin_output_path} {args.base_folder}/plugins"

    command = f"analyze_mutants {original_file} {compile_command} --noShuffle --verbose --timeout 120 --mutantDir {mutant_sources}"

    try:
        subprocess.run(command.split(" "))
    except Exception as e:
        print(e)