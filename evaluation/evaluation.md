# Evaluation instructions
----

The evaluation workflow consists of the following stages:
* Generate the mutant sources
* Discard irrelevant mutants
* Choose a number of mutants randomly
* Compile the chosen mutants

## Requirements

    python3+ universalmutator colourdiff

## Assumed direrctory structures
Imagine having a plugin project in the following structure:

    plugin/
        - CMakeLists.txt
        - src/
            - file1.cpp
            - file2.cpp
        - build/
            - bin/
                - plugin.vst3

The evaluation scripts assume the following structure:

    mutants/
        - file1/
            - src/
            - diff/
            - chosen_src/
            - chosen_diff/
            - original_file.txt
        - file2/
            - src/
            - diff/
            - chosen_src/
            - chosen_diff/
            - original_file.txt

This subfolder-structure below the `mutants/` folder must be identical for each file that should be mutated.

## Mutant sources generation
In this stage the goal is to obtain mutated sources based on one individual source file of the plugin.
Additionally coloured diffs will be generated that help with coarsly assessing whether mutants are suitable for further processing.
**Note that the paths in the example steps below will be shown with relative paths for readability. You should always use absolute paths!**

    # Generate the unchecked mutated sources
    $ python3 generate_mutants.py plugin/src/file1.cpp mutants/file1/

    $ ls mutants/file1/src
    file1.0.cpp 
    file1.1.cpp 
    file1.2.cpp
    ...
    
    $ ls mutants/file1/diff
    file1.0.cpp.diff
    file1.1.cpp.diff
    file1.2.cpp.diff
    ...

In the next step you can inspect the generated diffs and coarsely pre-select (= delete) mutants that would probably not compile or are not hit by the audio processing. If you have identified a mutant that is not relevant for the evaluation, just delete the mutant source file.

### Coverage reports
To generate coverage reports when using llvm/clang, the following steps are required:

* Use the following flags for **linking and compiling**: `-fprofile-instr-generate -fcoverage-mapping`
* Run the tests: `./reta fast ....`
* Find the generated coverage file `default.profraw` in the current directory
* Use llvm to parse this file into a different representation: `llvm-profdata merge -sparse default.profraw -o default.profdata`
* Print coverage report to console: `llvm-cov report path/to/plugin/binary -instr-profile=default.profdata`
* Generate html coverage report: `llvm-cov show --format=html path/to/plugin/binary -instr-profile=default.profdata > coverage.html`

Notes:
* It is possible to filter the coverage report using `-ignore-filename-regex=<PATTERN>` option to `llvm-cov`.
* On MacOS you might need to use the apple-clang tools: `/Library/Developer/CommandLineTools/usr/bin/llvm-profdata` and `/Library/Developer/CommandLineTools/usr/bin/llvm-cov`.
* On MacOS the `path/to/plugin/binary` must **not** be the path to the bundle (e.g. `plugin.vst3`) but rather the binary located in the bundle, e.g. `plugin.vst3/Contents/MacOS/plugin`.
* More information: https://llvm.org/docs/CommandGuide/llvm-cov.html

## Mutant source selection
Because the mutation tool generates a huge amount of mutations, it is necessary to select a subset of mutant sources to be used in the evaluation. In order to avoid a bias in the mutant selection, in this stage the mutant sources are presented in a random manner. The diff will be shown on the console to further improve the quality of the mutants. E.g. if a mutant that would fail to compile was missed in the previous stage, the mutant could still be skipped in this stage. As a result of this stage a chosen number of mutants will be placed in the `mutants/file1/chosen_src` folder, and the corresponding diffs will be placed in the `mutants/file1/chosen_diff` folder.
E.g. to choose 50 mutants randomly for file1, use:

    python3 choose-mutants.py plugin/src/file.cpp mutants/file1/src 50

## Mutant compilation

## Mutant analysis