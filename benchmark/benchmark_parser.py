import os
import re

import pandas as pd


def load_csv(filepath):
    data = pd.read_csv(filepath)
    return data


def get_csv_files(csv_dir='./'):
    csv = []
    for file in os.listdir(csv_dir):
        split = os.path.splitext(file)
        if split[1] == '.csv':
            alg = split[0].replace("benchmark-", '')
            csv.append((file, alg))
    return csv


def load_benchmarks(csv_dir='./'):
    """
    Creates a Pandas dataframe from all the 'benchmark-algo.csv' files in the dir.
    """
    benchmarks = pd.DataFrame(columns=['Dataset'])

    for (file, alg) in get_csv_files(csv_dir):
        bench = load_csv(file)

        if len(benchmarks.columns) == 1:
            benchmarks['Dataset'] = bench.iloc[:, 0]

        bench.columns = ['Dataset', alg]
        benchmarks = benchmarks.join(bench.set_index('Dataset'), on='Dataset')

    return benchmarks


def replace_digits(string):
    regex = r"\D+"
    value = re.search(regex, string).group(0)
    return value


def set_categories():
    global benchmarks
    benchmarks = benchmarks.assign(Category=lambda x: x.Dataset.apply(replace_digits))


def calculate_score():
    global benchmarks
    score = pd.DataFrame(benchmarks.iloc[:, 1:len(benchmarks.columns) - 1].idxmin(axis=1))
    score = score.rename(columns={0: 'Best'})
    score = score.join(benchmarks)
    score = score.groupby(['Category', 'Best'], as_index=False).count().iloc[:, 0:3]
    print(score.to_string())


if __name__ == '__main__':
    benchmarks = load_benchmarks()
    set_categories()
    calculate_score()
