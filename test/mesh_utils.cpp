#include <catch.hpp>

#include <mesh_util.hpp>

TEST_CASE( "Replicate vertices", "[replicate_vertices]" ) {
    SECTION("Empty mesh")
    {
        const auto replicated = merely3d::replicate_vertices(std::vector<float>(), std::vector<int>());
        REQUIRE(replicated.size() == 0);
    }

    SECTION("Single triangle")
    {
        const auto vertices = std::vector<float> {
            1.0, 2.0, 3.0,
            4.0, 5.0, 6.0,
            7.0, 8.0, 9.0
        };
        const auto replicated = merely3d::replicate_vertices(vertices, { 0, 1, 2 });
        REQUIRE(replicated == vertices);
    }

    SECTION("Two triangles")
    {
        const auto vertices = std::vector<float> {
                 1.0, 2.0, 3.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0,
                10.0, 11.0, 12.0,
        };
        const auto indices = std::vector<int> { 0, 1, 2, 1, 2, 3 };
        const auto replicated = merely3d::replicate_vertices(vertices, indices);

        const auto expected = std::vector<float> {
                 1.0, 2.0, 3.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0,
                10.0, 11.0, 12.0,
        };
        REQUIRE(replicated == expected);
    }
}

TEST_CASE("Unit sphere has normals equal to vertices", "[unit_sphere]")
{
    using Eigen::Vector3f;

    const auto v = merely3d::unit_sphere_vertices_and_normals();

    REQUIRE(v.size() % 6 * 3 == 0);

    const auto num_triangles = v.size() / 18;
    for (size_t t = 0; t < num_triangles; ++t)
    {
        const auto offset = 18 * t;
        const auto a = Vector3f(v[offset + 0], v[offset + 1], v[offset + 2]);
        const auto an = Vector3f(v[offset + 3], v[offset + 4], v[offset + 5]);
        const auto b = Vector3f(v[offset + 6], v[offset + 7], v[offset + 8]);
        const auto bn = Vector3f(v[offset + 9], v[offset + 10], v[offset + 11]);
        const auto c = Vector3f(v[offset + 12], v[offset + 13], v[offset + 14]);
        const auto cn = Vector3f(v[offset + 15], v[offset + 16], v[offset + 17]);

        REQUIRE(a.isApprox(an));
        REQUIRE(b.isApprox(bn));
        REQUIRE(c.isApprox(cn));
    }
}

TEST_CASE("Icosahedron replication", "[unit_sphere]")
{
    const auto vertices = merely3d::unit_icosahedron_vertices();
    const auto indices = merely3d::unit_icosahedron_indices();
    const auto replicated = merely3d::replicate_vertices(vertices, indices);

    const auto num_triangles = replicated.size() / 9;

    REQUIRE(replicated.size() % 9 == 0);
    REQUIRE(indices.size() % 3 == 0);
    REQUIRE(num_triangles == indices.size() / 3);
}
