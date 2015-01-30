Building LaTeX and HTML documentation:
--------------------------------------
(1) AMD API reference manual is built using doxygen, which is therefore
required to build the reference manual. The prescence of doxygen is checked
automatically when `./waf configure` is run, but doxygen is not required to
build AMD libraries or executables. On running `./waf doxygen`, the doxygen
detected at configure time is used to build our API reference manual. If
doxygen was not installed, this manual cannot be built.

(2) The design and tutorial directories are written entirely in LaTeX and
therefore, we need a functioning installation of LaTeX (typically from 
TeXLive). In addition, we also require users to have `tikz/pgf` package
installed to compile and build the figures. In addition, we use ImageMagick
and PNG to export TiKZ images to PNG images. In summary, to build design and
tutorial documentation, we need:
    -- LaTeX packages (pdflatex).
    -- ImageMagick, with PNG support built in.
    -- tex4ht (also called htlatex) for HTML documentation.

(3) Installing tex4ht:

(4) Installing ImageMagick:

(5) Fixing broken on your systems:
