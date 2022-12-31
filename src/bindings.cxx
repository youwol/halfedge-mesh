#include "halfedge.h"
#include <iostream>
#include <emscripten/bind.h>

template <typename T>
inline std::vector<T> valToArray(const emscripten::val& v, bool safe=false) {
    if (safe) {
        return emscripten::vecFromJSArray<T>(v) ;
    }

    // Optimize version but let unsafe than above
    const size_t l = v["length"].as<size_t>() ;
    std::vector<T> rv ;
    rv.resize(l) ;

    // Copy the array into our vector through the use of typed arrays.
    // It will try to convert each element through Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    emscripten::val memoryView{ emscripten::typed_memory_view(l, rv.data()) };
    memoryView.call<void>("set", v);

    //for (auto v: rv) std::cerr << v << "," ;
    //std::cerr << std::endl ;

    return rv;
}

template <typename T>
emscripten::val arrayToVal(const std::vector<T>& v) {
    return emscripten::val::array(v) ;
}

class Mesh {
public:
	Mesh(const emscripten::val& position, const emscripten::val& indices) {
        // TODO: optim afterward
        
        std::vector<std::vector<Halfedge_Mesh::Index>> polygons ;
        std::vector<Vec3> verts ;

        const std::vector<float>  pos = valToArray<float>(position) ;
        const std::vector<size_t> idx = valToArray<size_t>(indices) ;

        for (int i=0; i<pos.size(); i += 3) {
            verts.push_back( Vec3(pos[i], pos[i+1], pos[i+2]) ) ;
        }

        for (int i=0; i<idx.size(); i += 3) {
            polygons.push_back( {idx[i], idx[i+1], idx[i+2]} ) ;
        }

        mesh.from_poly(polygons, verts) ;
	}

    emscripten::val borders() {
        std::vector<float> b ;
        for (auto it = mesh.edges_begin(); it !=mesh.edges_end(); ++it) {
            if (it->on_boundary()) {
                const Vec3& v1 = it->halfedge()->vertex()->pos ;
                const Vec3& v2 = it->halfedge()->next()->vertex()->pos ;
                b.push_back(v1[0]); b.push_back(v1[1]); b.push_back(v1[2]) ;
                b.push_back(v2[0]); b.push_back(v2[1]); b.push_back(v2[2]) ;
            }
        }
        return arrayToVal(b) ;
    }

    emscripten::val borderIds() {
        std::vector<int> b ;
        for (auto v = mesh.vertices_begin(); v !=mesh.vertices_end(); ++v) {
            if (v->on_boundary()) {
                b.push_back( v->id() ) ;
                std::cerr << v->id() << " on border\n" ;
            }
            else {
                std::cerr << v->id() << "\n" ;
            }
            
        }
        std::cerr << "\n" ;
        return arrayToVal(b) ;
    }

    emscripten::val relax(int iteration) {
        for (int i=0; i<iteration; ++i) {
            for (auto v = mesh.vertices_begin(); v !=mesh.vertices_end(); ++v) {
                if (v->on_boundary() == false) {
                    Vec3 v0 = v->pos ;
                    Vec3 v1 = v->neighborhood_center() ;

                    //std::cerr << v0[0] << " " << v0[1] << " - " << v1[0] << " " << v1[1] << std::endl ;

                    v1 -= v0 ;
                    v1 /= 100 ;
                    v->pos += v1 ;
                }
            }
        }

        std::vector<float> positions ;
        for (auto it = mesh.vertices_begin(); it !=mesh.vertices_end(); ++it) {
            Vec3 v = it->pos ;
            positions.push_back(v[0]) ;
            positions.push_back(v[1]) ;
            positions.push_back(v[2]) ;
        }
        return arrayToVal(positions) ;
    }

    size_t nVertices() const {return mesh.n_vertices();}
    size_t nEdges() const {return mesh.n_edges();}
    size_t nFaces() const {return mesh.n_faces();}
    size_t nHalfedges() const {return mesh.n_halfedges();}
    size_t nBoundaries() const {return mesh.n_boundaries();}
    bool   hasBoundary() const {return mesh.has_boundary();}

private:
    Halfedge_Mesh mesh ;
} ;


emscripten::val borders(const emscripten::val& position, const emscripten::val& indices) {
    Mesh mesh(position, indices) ;
    return mesh.borders() ;
}

emscripten::val borderIds(const emscripten::val& position, const emscripten::val& indices) {
    Mesh mesh(position, indices) ;
    return mesh.borderIds() ;
}

emscripten::val relax(const emscripten::val& position, const emscripten::val& indices, int iteration) {
    Mesh mesh(position, indices) ;
    return mesh.relax(iteration) ;
}


EMSCRIPTEN_BINDINGS(mesh) {

    emscripten::register_vector<int>   ("StdVectorInt");
    emscripten::register_vector<double>("StdVectorDouble");

    emscripten::function("borders"  , borders) ;
    emscripten::function("borderIds", borderIds) ;
    emscripten::function("relax"    , relax) ;
}
