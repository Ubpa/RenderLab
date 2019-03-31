#ifndef _ENGINE_PRIMITIVE_TRIMESH_H_
#define _ENGINE_PRIMITIVE_TRIMESH_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/BVHNode.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Point2.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh : public Primitive {
			ELE_SETUP(TriMesh)
		public:
			enum ENUM_TYPE
			{
				INVALID,
				CODE, // default
				CUBE,
				SPHERE,
				PLANE,
				FILE,
			};
		public:
			TriMesh(const std::vector<uint> & indice,
				const std::vector<Pointf> & positions,
				const std::vector<Normalf> & normals,
				const std::vector<Point2f> & texcoords,
				ENUM_TYPE type = ENUM_TYPE::CODE);

			TriMesh(uint triNum, uint vertexNum,
				const uint * indice,
				const float * positions,
				const float * normals,
				const float * texcoords,
				ENUM_TYPE type = ENUM_TYPE::CODE);

		public:
			// 这个函数不应该主动调用
			void InitAfterGenSharePtr();

		public:
			ENUM_TYPE GetType() const { return type; }

			const std::vector<Pointf> & GetPositions() const { return positions; }
			const std::vector<Normalf> & GetNormals() const { return normals; }
			const std::vector<Point2f> & GetTexcoords() const { return texcoords; }
			const std::vector<Normalf> & GetTangents() const { return tangents; }
			const std::vector<uint> & GetIndice() const { return indice; }
			const std::vector<Triangle::Ptr> & GetTriangles() const { return triangles; }

		public:
			static TriMesh::Ptr GenCube();
			static TriMesh::Ptr GenSphere();
			static TriMesh::Ptr GenPlane();

		private:
			void GenTangents();

		private:
			ENUM_TYPE type;

			std::vector<uint> indice;
			std::vector<Pointf> positions;
			std::vector<Normalf> normals;
			std::vector<Point2f> texcoords;
			std::vector<Normalf> tangents;

			std::vector<Triangle::Ptr> triangles;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_TRIMESH_H_
