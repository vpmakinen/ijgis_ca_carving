Automatic determination of stream networks from DEMs by using road network data to locate culverts
================================================================================================================

Ville Mäkinen (a), Juha Oksanen (a), Tapani Sarjakoski (a)

(a) Finnish Geospatial Research Institute, Department of Geoinformatics and Cartography, National Land Survey of Finland, FI-02430 Masala, Finland

Corresponding author: Ville Mäkinen, ville.p.makinen@nls.fi

Abstract
--------
Determining stream networks automatically from digital elevation models is an issue that is actively being studied. As techniques for gathering elevation data have improved, the resolution and quality of digital elevation models have increased. Even so, many hydrologically critical features, such as culverts, are often missing from the elevation data. To analyze the surficial water flow, one must either prepare a special elevation model or post-process an already existing model.

This study builds on the traditional, well-established method of determining the stream network from digital elevation models. We have extended the traditional method by locating culverts automatically, using road network data as an input. We demonstrate that by including the automatically located culverts in the automatic stream network determination, the quality of the network can be noticeably improved.

Keywords: xxx

Citation
--------
Mäkinen, V., Oksanen, J. and T. Sarjakoski (submitted). Automatic determination of stream networks from DEMs by using road network data to locate culverts. DOI:xxx

Dependencies
------------

CMake (https://cmake.org)

Boost (https://www.boost.org)

GDAL (http://www.gdal.org)

Tested on the following environments:

    MacOS 10.12.6 - AppleClang 9.0.0.9000039 - Boost 1.66.0 - GDAL 2.1.3 - CMake 3.11.1

Installation
------------

cd *root directory of the project*

mkdir build

cd build

cmake ..

Sample data
-----------

Two data files need to be downloaded:

dem.gtiff (https://www.dropbox.com/s/2gl4um1xyzv0s7b/dem.gtiff?dl=0)
- Digital elevation model in 1 m grid. The model is derived from the aerial laser scanning data provided by the National Land Survey of Finland.
- MD5 sum: 4491928d9525511612544652e4f79ecd

roads.gtiff (https://www.dropbox.com/s/j8amrfsje4r0yo9/roads.gtiff?dl=0)
- Rasterised road vectors for the area. Vectors are taken from the Topographic Database of the National Land Survey of Finland.
- MD5 sum: daf1309b4d664e9e00c1fd0615a4c088

The laser scanning data and the Topographic Database are available at https://tiedostopalvelu.maanmittauslaitos.fi/tp/kartta?lang=en.

Running the program
-------------------

The results published in the article can be reproduced with the following command (in the build directory):

    ./bin/carving.bin --dem dem.gtiff --roads roads.gtiff --halo 500 --road-buffer 20 --ignore-dist-same-ite 200 --ignore-dist-other 10 --min-carving-cost 10 --min-single-carving 0.5 --min-flow-accumulation 10000

License
-------

The source code in the package is licensed under LGPL 2.1 license. See the file LICENCE.

The documentation in the package is available under the Creative Commons 4.0 BY ("Attribute") license. See https://creativecommons.org/licenses/by/4.0/legalcode
