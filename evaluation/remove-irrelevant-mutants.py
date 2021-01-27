import argparse, shutil, os

parser = argparse.ArgumentParser("remove-irrelevant-mutants")
parser.add_argument("irrelevant_txt", help="Path to the txt listing the irrelevant mutants")
parser.add_argument("--plugins", help="Path to the folder containing all plugins")
parser.add_argument("--sources", help="Path to the folder containing mutant sources")

args = parser.parse_args()

with open(args.irrelevant_txt) as file:
    irrelevant_ids = file.readlines()

if args.sources:
    for source in os.listdir(args.sources):
        id = source.split(".")[2]
        if id in irrelevant_ids:
            shutil.rmtree(args.sources + "/" + source)

if args.plugins:
    for plugin in os.listdir(args.plugins):
        if plugin.endswith(".vst3"):
            id = plugin.strip(".vst3").split("_")[1]
            if id in irrelevant_ids:
                shutil.rmtree(args.plugins + "/" + plugin)