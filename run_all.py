
import os
import time

# You will probably need to change these depending on what compiler you use.
compiler_path = '/usr/local/Cellar/llvm/14.0.6_1/bin/clang++'
compile_command = f'{compiler_path} -std=c++20 -Wall -Wextra -Werror -Wpedantic -O3'
days_completed = 17


def compile(file: str):
    os.system(f'{compile_command} {file} -o {file.removesuffix(".cpp")}')

def run(program: str, input: str):
    start = time.time()
    os.system(f'{program} < {input}')
    print(f'Runtime {round(time.time() - start, 3)}s')


def run_day(dir: str):
    print(f'====== {dir} Part 1 ======')
    compile(f'{dir}/part1.cpp')
    run(f'{dir}/part1', f'{dir}/input.txt')

    print(f'====== {dir} Part 2 ======')
    compile(f'{dir}/part2.cpp')
    run(f'{dir}/part2', f'{dir}/input.txt')


def main():
    for i in range(1, days_completed + 1):
        run_day(f'Day{i:02}')


if __name__ == '__main__':
    main()
