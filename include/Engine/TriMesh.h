#pragma once

#include <Engine/Primitive.h>
#include <Engine/Triangle.h>

#include <UGM/normal.h>
#include <UGM/point.h>

#include <vector>

namespace CppUtil {
	namespace Engine {
		class TriMesh final : public Primitive {
		public:
			enum ENUM_TYPE
			{
				INVALID,
				CODE, // default
				CUBE,
				SPHERE,
				PLANE,
				DISK,
			};
		public:
			TriMesh(const std::vector<unsigned> & indice,
				const std::vector<Ubpa::pointf3> & positions,
				const std::vector<Ubpa::normalf> & normals,
				const std::vector<Ubpa::pointf2> & texcoords,
				const std::vector<Ubpa::normalf> & tangents = std::vector<Ubpa::normalf>(),
				ENUM_TYPE type = ENUM_TYPE::CODE)
			{
				Init(true, indice, positions, normals, texcoords, tangents, type);
			}

			TriMesh(unsigned triNum, unsigned vertexNum,
				const unsigned * indice,
				const float * positions,
				const float * normals,
				const float * texcoords,
				const float * tangents = nullptr,
				ENUM_TYPE type = ENUM_TYPE::CODE);

		public:
			static const Basic::Ptr<TriMesh> New(const std::vector<unsigned> & indice,
				const std::vector<Ubpa::pointf3> & positions,
				const std::vector<Ubpa::normalf> & normals = std::vector<Ubpa::normalf>(),
				const std::vector<Ubpa::pointf2> & texcoords = std::vector<Ubpa::pointf2>(),
				const std::vector<Ubpa::normalf> & tangents = std::vector<Ubpa::normalf>(),
				ENUM_TYPE type = ENUM_TYPE::CODE) {
				return Basic::New<TriMesh>(indice, positions, normals, texcoords, tangents, type);
			}

			static const Basic::Ptr<TriMesh> New(unsigned triNum, unsigned vertexNum,
				const unsigned * indice,
				const float * positions,
				const float * normals,
				const float * texcoords,
				const float * tangents = nullptr,
				ENUM_TYPE type = ENUM_TYPE::CODE) {
				return Basic::New<TriMesh>(triNum, vertexNum, indice, positions, normals, texcoords, tangents, type);
			}

		public:
			void Init(const std::vector<unsigned> & indice,
				const std::vector<Ubpa::pointf3> & positions,
				const std::vector<Ubpa::normalf> & normals = std::vector<Ubpa::normalf>(),
				const std::vector<Ubpa::pointf2> & texcoords = std::vector<Ubpa::pointf2>(),
				const std::vector<Ubpa::normalf> & tangents = std::vector<Ubpa::normalf>(),
				ENUM_TYPE type = ENUM_TYPE::CODE)
			{
				Init(false, indice, positions, normals, texcoords, tangents, type);
			}

			bool Update(const std::vector<Ubpa::pointf3> & positions);
			bool Update(const std::vector<Ubpa::pointf2> & texcoords);

		private:
			void Init(bool creator, const std::vector<unsigned> & indice,
				const std::vector<Ubpa::pointf3> & positions,
				const std::vector<Ubpa::normalf> & normals = std::vector<Ubpa::normalf>(),
				const std::vector<Ubpa::pointf2> & texcoords = std::vector<Ubpa::pointf2>(),
				const std::vector<Ubpa::normalf> & tangents = std::vector<Ubpa::normalf>(),
				ENUM_TYPE type = ENUM_TYPE::CODE);

		private:
			virtual ~TriMesh() = default;

		private:
			virtual void Init_AfterGenPtr() override;

		public:
			ENUM_TYPE GetType() const { return type; }

			const std::vector<Ubpa::pointf3> & GetPositions() const { return positions; }
			const std::vector<Ubpa::normalf> & GetNormals() const { return normals; }
			const std::vector<Ubpa::pointf2> & GetTexcoords() const { return texcoords; }
			const std::vector<Ubpa::normalf> & GetTangents() const { return tangents; }
			const std::vector<unsigned> & GetIndice() const { return indice; }
			const std::vector<Basic::Ptr<Triangle>> & GetTriangles() const { return triangles; }
			std::vector<Ubpa::pointf3> & GetPositions() { return positions; }
			std::vector<Ubpa::normalf> & GetNormals() { return normals; }
			std::vector<Ubpa::pointf2> & GetTexcoords() { return texcoords; }
			std::vector<Ubpa::normalf> & GetTangents() { return tangents; }
			std::vector<unsigned> & GetIndice() { return indice; }
			std::vector<Basic::Ptr<Triangle>> & GetTriangles() { return triangles; }

		public:
			virtual const Ubpa::bboxf3 GetBBox() const override {
				return box;
			}

		public:
			static const Basic::Ptr<TriMesh> GenCube();
			static const Basic::Ptr<TriMesh> GenSphere();
			static const Basic::Ptr<TriMesh> GenPlane();
			static const Basic::Ptr<TriMesh> GenDisk();

		private:
			void GenTangents();
			void GenNormals();

		private:
			ENUM_TYPE type;

			std::vector<unsigned> indice;
			std::vector<Ubpa::pointf3> positions;
			std::vector<Ubpa::normalf> normals;
			std::vector<Ubpa::pointf2> texcoords;
			std::vector<Ubpa::normalf> tangents;

			std::vector<Basic::Ptr<Triangle>> triangles;

			Ubpa::bboxf3 box;
		};
	}
}
