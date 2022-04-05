from setuptools import setup, Extension
from pathlib import Path
from os import walk
import os


def get_includes() -> [str]:
        return [f'{os.getcwd()}{os.sep}include']


def get_sources(current_path: str, extension: tuple[str, ...] | str = '') -> [str]:
    ret = []
    for root, _, files in walk(current_path):
        for file in files:
            path_str = f'{root}{os.sep}{file}'
            path = Path(path_str)
            if path.is_file() and path.suffix.lower().endswith(extension):
                ret.append(path_str)

    return ret

def main():
        setup(
            name="PyInt64", 
            version='0.1',
            description='Wow',
            author='GTerm',
            author_email='abc@abc.com',
            url='www.GTerm.com',
            license='LICENSE',
            ext_modules=[
                Extension('pyint64', get_sources(os.getcwd(), ('.cpp', '.c', '.cc', '.cxx')), get_includes())
            ]
        )


if __name__ == '__main__':
        main()