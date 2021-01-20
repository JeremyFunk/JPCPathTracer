#include "gtest/gtest.h"
#include <memory>
#include <vector>
#include "bsdfs/LambertianBSDF.h"
#include "core/Bsdf.h"
#include "core/BsdfGeneric.h"
#include "core/Linalg.h"

std::vector<float> rand_point_x()
{
    return {
        0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001
        ,0.05
        ,0.15000000000000002
        ,0.25
        ,0.35
        ,0.45
        ,0.55
        ,0.65
        ,0.75
        ,0.8500000000000001
        ,0.9500000000000001

    };
}

std::vector<float> rand_point_y()
{
    return {
    0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.05
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.15000000000000002
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.25
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.35
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.45
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.55
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.65
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.75
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.8500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001
    ,0.9500000000000001

    };
}

std::vector<float> sample_x()
{
    return 
    {-0.6363961030678928
    ,-0.5162187927159415
    ,-0.3803564355666295
    ,-0.23293714059226886
    ,-0.07844016847289233
    ,0.07844016847289241
    ,0.23293714059226878
    ,0.3803564355666294
    ,0.5162187927159415
    ,0.636396103067893
    ,-0.7372368398600926
    ,-0.4949747468305833
    ,-0.37242245356073556
    ,-0.231195343368617
    ,-0.07837513327231553
    ,0.07837513327231546
    ,0.23119534336861688
    ,0.3724224535607355
    ,0.4949747468305835
    ,0.7372368398600928
    ,-0.815677008332985
    ,-0.5927069394597988
    ,-0.3535533905932738
    ,-0.2269952498697734
    ,-0.07821723252011546
    ,0.07821723252011552
    ,0.22699524986977335
    ,0.35355339059327373
    ,0.5927069394597991
    ,0.8156770083329851
    ,-0.8693332436601615
    ,-0.6607183312158572
    ,-0.44550326209418395
    ,-0.2121320343559643
    ,-0.07764571353075624
    ,0.07764571353075626
    ,0.21213203435596428
    ,0.44550326209418395
    ,0.6607183312158574
    ,0.8693332436601616
    ,-0.896575228282571
    ,-0.6955985469252698
    ,-0.4938441702975689
    ,-0.28977774788672056
    ,-0.07071067811865474
    ,0.07071067811865488
    ,0.28977774788672056
    ,0.4938441702975689
    ,0.69559854692527
    ,0.8965752282825711
    ,-0.896575228282571
    ,-0.6955985469252698
    ,-0.49384417029756883
    ,-0.2897777478867205
    ,-0.07071067811865474
    ,0.07071067811865482
    ,0.2897777478867205
    ,0.49384417029756883
    ,0.69559854692527
    ,0.8965752282825711
    ,-0.8693332436601615
    ,-0.6607183312158572
    ,-0.44550326209418395
    ,-0.21213203435596428
    ,-0.0776457135307562
    ,0.0776457135307563
    ,0.2121320343559643
    ,0.44550326209418395
    ,0.6607183312158574
    ,0.8693332436601616
    ,-0.815677008332985
    ,-0.5927069394597988
    ,-0.35355339059327373
    ,-0.22699524986977335
    ,-0.0782172325201154
    ,0.07821723252011557
    ,0.2269952498697734
    ,0.3535533905932738
    ,0.5927069394597991
    ,0.8156770083329851
    ,-0.7372368398600926
    ,-0.49497474683058335
    ,-0.3724224535607356
    ,-0.23119534336861694
    ,-0.07837513327231548
    ,0.07837513327231556
    ,0.23119534336861708
    ,0.3724224535607357
    ,0.4949747468305834
    ,0.7372368398600927
    ,-0.6363961030678928
    ,-0.5162187927159413
    ,-0.38035643556662946
    ,-0.2329371405922686
    ,-0.07844016847289223
    ,0.07844016847289254
    ,0.2329371405922687
    ,0.38035643556662957
    ,0.5162187927159416
    ,0.636396103067893

    };
}

std::vector<float> sample_y()
{
    return {
        -0.6363961030678927
        ,-0.7372368398600926
        ,-0.815677008332985
        ,-0.8693332436601614
        ,-0.896575228282571
        ,-0.896575228282571
        ,-0.8693332436601615
        ,-0.8156770083329851
        ,-0.7372368398600926
        ,-0.6363961030678928
        ,-0.5162187927159415
        ,-0.4949747468305832
        ,-0.5927069394597988
        ,-0.6607183312158572
        ,-0.6955985469252698
        ,-0.6955985469252698
        ,-0.6607183312158572
        ,-0.5927069394597989
        ,-0.4949747468305833
        ,-0.5162187927159414
        ,-0.3803564355666295
        ,-0.37242245356073556
        ,-0.35355339059327373
        ,-0.4455032620941839
        ,-0.4938441702975689
        ,-0.49384417029756883
        ,-0.44550326209418395
        ,-0.3535533905932738
        ,-0.3724224535607356
        ,-0.3803564355666295
        ,-0.23293714059226872
        ,-0.23119534336861694
        ,-0.2269952498697734
        ,-0.21213203435596428
        ,-0.28977774788672056
        ,-0.28977774788672056
        ,-0.2121320343559643
        ,-0.2269952498697734
        ,-0.231195343368617
        ,-0.2329371405922687
        ,-0.07844016847289234
        ,-0.07837513327231549
        ,-0.07821723252011542
        ,-0.07764571353075622
        ,-0.07071067811865472
        ,-0.07071067811865475
        ,-0.07764571353075622
        ,-0.07821723252011542
        ,-0.07837513327231549
        ,-0.07844016847289234
        ,0.07844016847289242
        ,0.07837513327231557
        ,0.0782172325201155
        ,0.0776457135307563
        ,0.07071067811865489
        ,0.07071067811865481
        ,0.0776457135307563
        ,0.0782172325201155
        ,0.07837513327231557
        ,0.07844016847289242
        ,0.23293714059226872
        ,0.23119534336861694
        ,0.2269952498697734
        ,0.2121320343559643
        ,0.28977774788672056
        ,0.2897777478867205
        ,0.21213203435596428
        ,0.2269952498697734
        ,0.231195343368617
        ,0.2329371405922687
        ,0.3803564355666295
        ,0.37242245356073556
        ,0.3535533905932738
        ,0.44550326209418395
        ,0.4938441702975689
        ,0.49384417029756883
        ,0.4455032620941839
        ,0.35355339059327373
        ,0.3724224535607356
        ,0.3803564355666295
        ,0.5162187927159415
        ,0.4949747468305834
        ,0.5927069394597991
        ,0.6607183312158574
        ,0.69559854692527
        ,0.69559854692527
        ,0.6607183312158574
        ,0.592706939459799
        ,0.49497474683058335
        ,0.5162187927159416
        ,0.636396103067893
        ,0.7372368398600929
        ,0.8156770083329852
        ,0.8693332436601616
        ,0.8965752282825711
        ,0.8965752282825711
        ,0.8693332436601616
        ,0.8156770083329851
        ,0.7372368398600926
        ,0.6363961030678928

    };
}

std::vector<float> sample_z()
{
    return {
    0.43588989435406733
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.4358898943540673
    ,0.435889894354067
    ,0.4358898943540673
    ,0.7141428428542851
    ,0.7141428428542852
    ,0.714142842854285
    ,0.714142842854285
    ,0.714142842854285
    ,0.7141428428542852
    ,0.714142842854285
    ,0.7141428428542849
    ,0.43588989435406716
    ,0.4358898943540673
    ,0.7141428428542852
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.7141428428542849
    ,0.43588989435406716
    ,0.4358898943540673
    ,0.714142842854285
    ,0.8660254037844386
    ,0.9539392014169457
    ,0.9539392014169457
    ,0.9539392014169457
    ,0.9539392014169457
    ,0.8660254037844386
    ,0.7141428428542849
    ,0.43588989435406705
    ,0.43588989435406733
    ,0.714142842854285
    ,0.8660254037844386
    ,0.9539392014169457
    ,0.99498743710662
    ,0.99498743710662
    ,0.9539392014169457
    ,0.8660254037844386
    ,0.7141428428542849
    ,0.4358898943540672
    ,0.43588989435406733
    ,0.714142842854285
    ,0.8660254037844386
    ,0.9539392014169457
    ,0.99498743710662
    ,0.99498743710662
    ,0.9539392014169457
    ,0.8660254037844386
    ,0.7141428428542849
    ,0.4358898943540672
    ,0.4358898943540673
    ,0.714142842854285
    ,0.8660254037844386
    ,0.9539392014169457
    ,0.9539392014169457
    ,0.9539392014169457
    ,0.9539392014169457
    ,0.8660254037844386
    ,0.7141428428542849
    ,0.43588989435406705
    ,0.4358898943540673
    ,0.7141428428542852
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.8660254037844386
    ,0.7141428428542849
    ,0.43588989435406716
    ,0.4358898943540673
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.7141428428542849
    ,0.43588989435406705
    ,0.43588989435406705
    ,0.43588989435406705
    ,0.43588989435406705
    ,0.43588989435406705
    ,0.4358898943540673
    ,0.43588989435406716
    ,0.43588989435406705
    ,0.43588989435406716
    ,0.43588989435406716
    ,0.435889894354067
    };
}

TEST(lambertian,val1)
{
    auto closure = std::make_shared<bsdfs::LambertianBSDF>();
    core::BsdfMemoryInfo info = {0,0};
    info.max_bsdf_count = 1;
    
    auto bsdf = std::make_shared<core::BsdfGeneric<bsdfs::LambertianParams>>(closure);

    info.max_byte_size = bsdf->GetMaxSize();

    auto memory = core::CreateBsdfMemory(info);

    core::ResetBsdfMemory(memory, {0,0,1});

    bsdf->Setup(memory, 1);

    core::Vec3 direction = {0,0,0};

    auto v_x = sample_x();
    auto v_y = sample_y();
    auto v_z = sample_z();
    auto s_x = rand_point_x();
    auto s_y = rand_point_y();

    

    for(int i = 0; i< v_x.size();i++)
    {
        core::Vec2 rand_point = {s_x[i],s_y[i]};
        core::Vec3 sample = core::SampleIncidentDirectionBsdf(memory, direction, rand_point);
        EXPECT_NEAR(v_x[i],sample[0],0.001);
        EXPECT_NEAR(v_y[i],sample[1],0.001);
        EXPECT_NEAR(v_z[i],sample[2],0.001);
    }
}