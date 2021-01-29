import argparse
import os
import subprocess
import tempfile
from multiprocessing.pool import ThreadPool
from threading import Lock

parser = argparse.ArgumentParser("analyze-mutants")
parser.add_argument("reta", help="path to reta")
parser.add_argument("config", help="path to config")
parser.add_argument("reference", help="path to reference plugin")
parser.add_argument("mutants", help="path to mutant folder")
args = parser.parse_args()


def cmd(command):
    return subprocess.run(command.split(" "), timeout=600, capture_output=True)


def test_mutant(current_mutant, actual):

    try:
        cmd(f"{args.reta} test -c {args.config} -i {args.mutants}/{current_mutant} -o {actual}")
        process = cmd(
            f"{args.reta} compare -r {reference} -a {actual} -o foobar.json")
        mutex.acquire()
        not_killed.append(
            current_mutant) if process.returncode == 0 else killed.append(current_mutant)
        print_stats()
        mutex.release()

    except KeyboardInterrupt:
        exit()
    except Exception as e:
        mutex.acquire()
        killed.append(current_mutant)
        mutex.release()
        return True


def print_stats():
    done = len(killed) + len(not_killed)
    print(f"Done: {done}/{total} ({round(done * 100 / total, 2)}%), Score: {round(len(killed) * 100 / done, 2)}", end="\r")


def write_stats():
    with open("killed.txt", "w") as file:
        file.write("\n".join(killed))
    with open("not_killed.txt", "w") as file:
        file.write("\n".join(not_killed))


def worker(current_mutant):
    with tempfile.TemporaryDirectory() as actual:
        return test_mutant(current_mutant, actual)


if __name__ == '__main__':
    mutants = os.listdir(args.mutants)
    mutants.sort()

    mutex = Lock()
    killed = []
    not_killed = []
    total = len(mutants)

    with tempfile.TemporaryDirectory() as reference:
        print("Testing reference: " + args.reference)
        cmd(f"{args.reta} test -c {args.config} -i {args.reference} -o {reference}")

        with ThreadPool(10) as pool:
            pool.map(worker, mutants)

    print()
    print_stats()
    print()
    write_stats()
