#pragma once

#include "Primitive.h"

#include "Triangle.h"

#include <UGM/normal.h>
#include <UGM/point.h>

#include <vector>

namespace Ubpa {
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
		TriMesh(const std::vector<unsigned>& indice,
			const std::vector<pointf3>& positions,
			const std::vector<normalf>& normals,
			const std::vector<pointf2>& texcoords,
			const std::vector<normalf>& tangents = std::vector<normalf>(),
			ENUM_TYPE type = ENUM_TYPE::CODE)
		{
			Init(true, indice, positions, normals, texcoords, tangents, type);
		}

		TriMesh(unsigned triNum, unsigned vertexNum,
			const unsigned* indice,
			const float* positions,
			const float* normals,
			const float* texcoords,
			const float* tangents = nullptr,
			ENUM_TYPE type = ENUM_TYPE::CODE);

	public:
		static const Ptr<TriMesh> New(const std::vector<unsigned>& indice,
			const std::vector<pointf3>& positions,
			const std::vector<normalf>& normals = std::vector<normalf>(),
			const std::vector<pointf2>& texcoords = std::vector<pointf2>(),
			const std::vector<normalf>& tangents = std::vector<normalf>(),
			ENUM_TYPE type = ENUM_TYPE::CODE) {
			return Ubpa::New<TriMesh>(indice, positions, normals, texcoords, tangents, type);
		}

		static const Ptr<TriMesh> New(unsigned triNum, unsigned vertexNum,
			const unsigned* indice,
			const float* positions,
			const float* normals,
			const float* texcoords,
			const float* tangents = nullptr,
			ENUM_TYPE type = ENUM_TYPE::CODE) {
			return Ubpa::New<TriMesh>(triNum, vertexNum, indice, positions, normals, texcoords, tangents, type);
		}

	public:
		void Init(const std::vector<unsigned>& indice,
			const std::vector<pointf3>& positions,
			const std::vector<normalf>& normals = std::vector<normalf>(),
			const std::vector<pointf2>& texcoords = std::vector<pointf2>(),
			const std::vector<normalf>& tangents = std::vector<normalf>(),
			ENUM_TYPE type = ENUM_TYPE::CODE)
		{
			Init(false, indice, positions, normals, texcoords, tangents, type);
		}

		bool Update(const std::vector<pointf3>& positions);
		bool Update(const std::vector<pointf2>& texcoords);

	private:
		void Init(bool creator, const std::vector<unsigned>& indice,
			const std::vector<pointf3>& positions,
			const std::vector<normalf>& normals = std::vector<normalf>(),
			const std::vector<pointf2>& texcoords = std::vector<pointf2>(),
			const std::vector<normalf>& tangents = std::vector<normalf>(),
			ENUM_TYPE type = ENUM_TYPE::CODE);

	private:
		virtual ~TriMesh() = default;

	private:
		virtual void Init_AfterGenPtr() override;

	public:
		ENUM_TYPE GetType() const { return type; }

		const std::vector<pointf3>& GetPositions() const { return positions; }
		const std::vector<normalf>& GetNormals() const { return normals; }
		const std::vector<pointf2>& GetTexcoords() const { return texcoords; }
		const std::vector<normalf>& GetTangents() const { return tangents; }
		const std::vector<unsigned>& GetIndice() const { return indice; }
		const std::vector<Ptr<Triangle>>& GetTriangles() const { return triangles; }
		std::vector<pointf3>& GetPositions() { return positions; }
		std::vector<normalf>& GetNormals() { return normals; }
		std::vector<pointf2>& GetTexcoords() { return texcoords; }
		std::vector<normalf>& GetTangents() { return tangents; }
		std::vector<unsigned>& GetIndice() { return indice; }
		std::vector<Ptr<Triangle>>& GetTriangles() { return triangles; }

	public:
		virtual const bboxf3 GetBBox() const override {
			return box;
		}

	public:
		static const Ptr<TriMesh> GenCube();
		static const Ptr<TriMesh> GenSphere();
		static const Ptr<TriMesh> GenPlane();
		static const Ptr<TriMesh> GenDisk();

	private:
		void GenTangents();
		void GenNormals();

	private:
		ENUM_TYPE type;

		std::vector<unsigned> indice;
		std::vector<pointf3> positions;
		std::vector<normalf> normals;
		std::vector<pointf2> texcoords;
		std::vector<normalf> tangents;

		std::vector<Ptr<Triangle>> triangles;

		bboxf3 box;
	};
}
