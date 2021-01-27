import argparse, os, shutil, subprocess

parser = argparse.ArgumentParser("analyze-mutants")
parser.add_argument("reta", help="path to reta")
parser.add_argument("config", help="path to config")
parser.add_argument("mutants", help="path to mutant folder")
parser.add_argument("reference", help="path to reference output")
parser.add_argument("tmp", help="path to temporary folder")
args = parser.parse_args()

mutants = os.listdir(args.mutants)
mutants.sort()

killed = []
notkilled = []
total = len(mutants)

for mutant in mutants:

    print("Testing : "+ mutant)
    
    shutil.rmtree(args.tmp)
    os.mkdir(args.tmp)

    try:
        subprocess.run(f"{args.reta} test -c {args.config} -i {args.mutants}/{mutant} -o {args.tmp}".split(" "), timeout=600)
        process = subprocess.run(f"{args.reta} compare -r {args.reference} -l {args.tmp} -o foobar.json".split(" "), timeout=600)
        notkilled.append(mutant) if process.returncode == 0 else killed.append(mutant)
    except KeyboardInterrupt:
        exit()
    except:
        killed.append(mutant)
    
    num_processed = len(killed) + len(notkilled)
    print(f"Mutant {num_processed}/{total} ({num_processed / total}%)")
    print(f"Running Score: {len(killed) * 100 / num_processed}")

with open("killed.txt", "w") as file:
    file.write("\n".join(killed))
with open("notkilled.txt", "w") as file:
    file.write("\n".join(notkilled))