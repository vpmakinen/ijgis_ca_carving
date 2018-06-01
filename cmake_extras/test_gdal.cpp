#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include <iostream>

int main(int /*argc*/, char** argv) {
    GDALAllRegister();
    GDALDataset * dataset;
    OGRLayer * layer;
    const char *pszDriverName = "ESRI Shapefile";
    GDALDriver * driver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
    if (!driver) {
        throw std::runtime_error("Driver 'ESRI Shapefile' not available.");
    }
    dataset = static_cast<GDALDataset*>(GDALOpenEx(argv[1],
        GDAL_OF_VECTOR, 0, 0, 0));
    if (!dataset) {
        return 1;
    }
    layer = dataset->GetLayerByName("test_gdal_data");
    layer->ResetReading();

    int n {0};
    while (true)
    {
        auto * feature = layer->GetNextFeature();
        if (!feature) break;
        ++n;
        OGRFeature::DestroyFeature(feature);
    }
    if (n != 4) {
        return 2;
    }
    GDALClose(dataset);
    #ifdef CMAKE_TRY_RUN
    std::cout << "SUCCESS" << std::endl;
    #endif
    return 0;
}
