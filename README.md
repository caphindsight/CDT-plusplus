CDT-plusplus
============

Causal Dynamical Triangulations in C++ using the [Computational Geometry Algorithms Library][1] and compiled with
[CMake][2] using [Clang][8]/[LLVM][3]. Follows (mostly) the [Google C++ Style Guide][14], which you can check by
running the (included in project) [cpplint.py][15] script:

```
# python cpplint.py <filename>
```

The goals and targets of this project are:

- [x] 3D Simplex
- [x] 3D Spherical simplicial complex
- [ ] 3D Toroidal simplicial complex
- [ ] 2+1 foliation
- [ ] 4D Simplex
- [ ] 4D Simplicial complex
- [ ] 3+1 foliation
- [x] Tests using [CTest][4]
- [ ] Complete test coverage
- [ ] Test builds with [CDash][5]
- [x] [Literate programming][6] generated using [Doxygen][7]
- [ ] Complete documentation
- [ ] Initialize two masses
- [ ] Shortest path algorithm
- [ ] Ergodic moves
- [ ] Metropolis algorithm
- [ ] Einstein tensor
- [ ] ???
- [ ] (Non)profit

Build:
------

CDT++ should build on any system (e.g. Linux, MacOS, Windows) with [CMake][9] and [CGAL][10] installed. 

On MacOS, the easiest way to do this is with [HomeBrew][13]:

```
# brew install cmake
# brew install eigen
# brew install cgal --imaging --with-eigen3 --with-lapack
```

On Ubuntu, you can do this via:
```
# sudo apt-get install cmake
# sudo apt-get install libcgal-dev
```

Then, download this source code (clone this repo from GitHub or grab a zipped archive [here][12]) and run the following commands in the top-level directory:

```
# cmake .
# make
```

There are also directions for doing a [parallel build using CMake][11], but currently the builds don't really take long enough to bother.

Unit tests:
-----------

You can build and run unit tests by typing:

```
# make tests
```

In addition to the command line output, you can see detailed results in the Testing directory which is generated thereby.

Usage:
------

cdt (number of simplices)

Currently defaults to making random spherical 3D simplicial manifolds

Documentation:
--------------

If you have Doxygen installed you can simply type:

```
# doxygen
```

This will generate html/ and latex/ directories which will contain documentation generated from CDT++ source files. USE_MATHJAX has been enabled in Doxyfile so that the LaTeX formulae can be rendered in the html documentation using [MathJax][16]. HAVE_DOT is set to YES which allows various graphs to be autogenerated by Doxygen using [GraphViz][17]. If you do not have GraphViz installed, set this option to NO.


[1]: http://www.cgal.org
[2]: http://www.cmake.org
[3]: http://llvm.org 
[4]: http://cmake.org/Wiki/CMake/Testing_With_CTest
[5]: http://open.cdash.org/index.php
[6]: http://www.literateprogramming.com
[7]: http://www.doxygen.org
[8]: http://clang.llvm.org
[9]: http://www.cmake.org/cmake/help/install.html
[10]: http://www.cgal.org/Manual/latest/doc_html/installation_manual/Chapter_installation_manual.html
[11]: http://www.kitware.com/blog/home/post/434
[12]: https://github.com/acgetchell/CDT-plusplus/archive/master.zip
[13]: http://brew.sh
[14]: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
[15]: http://google-styleguide.googlecode.com/svn/trunk/cpplint/cpplint.py
[16]: http://www.mathjax.org
[17]: http://www.graphviz.org
