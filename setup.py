import io
import os
from setuptools import setup, Extension, find_packages
import pybind11

# Read the README for the long description
with io.open(os.path.join(os.path.dirname(__file__), 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

ext_modules = [
    Extension(
        'markdown_break._chunker',
        sources=[
            'py_markdown_chunker.cpp',
            'markdown_chunker.cpp',
        ],
        include_dirs=[pybind11.get_include()],
        language='c++',
        extra_compile_args=['-std=c++17'],
    ),
]

setup(
    name='markdown-break',
    version='0.1.0',
    author='Your Name',
    author_email='you@example.com',
    description='Break markdown text into semantic chunks via C++ extension',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/yourusername/markdown-break',
    packages=find_packages(where='src'),
    package_dir={'': 'src'},
    ext_modules=ext_modules,
    python_requires='>=3.10',
    install_requires=['pybind11>=2.13.6'],
    extras_require={'test': ['pytest', 'pytest-cov']},
    entry_points={'console_scripts': ['markdown-break=markdown_break.cli:main']},
    classifiers=[
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.10',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    zip_safe=False,
)