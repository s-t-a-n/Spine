"""Sphinx configuration."""
project = "Spine"
author = "s-t-a-n"
copyright = "2024, Stan Verschuuren"
extensions = [
    "sphinx_rtd_theme",
    "sphinxcontrib.doxylink"
]
autodoc_typehints = "description"

doxygen_root = "docs/doxygen"  # this is just a convenience variable
doxylink = {
    "demo": (
        # "demo" is the role name that you can later use in sphinx to reference this doxygen documentation (see below)
        f"{doxygen_root}/demo/html/tagfile.xml",  # the first parameter of this tuple is the tagfile
        f"{doxygen_root}/demo/html",  # the second parameter of this tuple is a relative path pointing from
        # sphinx output directory to the doxygen output folder inside the output
        # directory tree.
        # Doxylink will use the tagfile to get the html file name of the symbol you want
        # to link and then prefix it with this path to generate html links (<a>-tags).
    ),
}

templates_path = ['_templates']
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output
html_theme = "sphinx_rtd_theme"
html_static_path = ['_static']
