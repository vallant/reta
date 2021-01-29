
import argparse, subprocess, shutil, os


def get_current_mutant_filename(mutants):
    # the analyze-mutants command from universalmutator generates the killed.txt and notkilled.txt files
    # the sum of all killed (=not-compiling) and not-killed (compiling) mutants indicates the current index 
    with open("killed.txt") as file:
        mutant_index = len(file.readlines())
    with open("notkilled.txt") as file:
        mutant_index = mutant_index + len(file.readlines())
    return mutants[mutant_index]


def get_current_mutant_name(mutants, plugin_name):
    # mutant sources have the format
    # <filename>.mutant.<id>.(h|cpp)
    mutant_filename = get_current_mutant_filename(mutants)
    filename_base = mutant_filename.split(".")[0]
    mutant_id = mutant_filename.split(".")[2]

    return plugin_name + "_" + filename_base + "_" + mutant_id


parser = argparse.ArgumentParser("compile-mutant")
parser.add_argument("working_directory", help="Absolute path to working directory where the compile command should be executed (relative to current folder)")
parser.add_argument("mutant_sources", help="Absolute path to folder containing mutant sources")
parser.add_argument("compile_command", help="Command used to compile the plugin")
parser.add_argument("plugin_output_path", help="Absolute path where the compiled plugin is located, including extension (e.g. bin/foo.vst3)")
parser.add_argument("output_folder", help="Absolute path to output folder for mutant")

args = parser.parse_args()

# e.g. frequalizer
plugin_name = os.path.basename(args.plugin_output_path).split(".")[0]

# e.g. .vst3
extension = args.plugin_output_path.split(".")[-1]


mutants = os.listdir(args.mutant_sources)

# e.g. foo.bar.0
mutant_plugin_name = get_current_mutant_name(mutants, plugin_name)



# e.g. build/mutants/foo.bar.0.vst3
output_path = args.output_folder + "/" + mutant_plugin_name + "." + extension

# only compile plugins that aren't compiled yet
if not os.path.exists(output_path):

    # compile plugin
    subprocess.run(args.compile_command.split(" "), cwd=args.working_directory)

    # copy to final destination
    shutil.copytree(args.plugin_output_path, output_path)




