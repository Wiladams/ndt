#pragma once

#include <cstdio>
#include <ctype.h>

#include "trianglemesh.h"
#include "filestream.h"


// typical file structure
// solid OpenSCAD_Model
//   facet normal f1 f2 f3
//     outer loop
//       vertex f1 f2 f3
//       vertex f1 f2 f3
//       vertex f1 f2 f3
//     endloop
//   endfacet
// endsolid OpenSCAD_Model
namespace stl {

	struct Token {
		char* data;
		size_t size;

		Token() :data(nullptr), size(0) {}

		std::string toString()
		{
			return std::string(data, size);
		}

		float toFloat()
		{
			return atof(toString().c_str());
		}
	};

	bool skipspaces(BinStream& bs)
	{
		while (true) {
			if (bs.isEOF()) {
				return false;
			}

			if (isspace(bs.peekOctet()))
			{
				bs.skip(1);
			} else {
				return true;
			}
		}
	}

	Token nextToken(BinStream& bs)
	{
		if (!skipspaces(bs))
			return Token();

		// read until a space character, or EOF
		size_t ending = bs.tell();
		size_t starting = bs.tell();
		Token tok;
		tok.data = (char *)bs.getPositionPointer();

		while (true) {
			int c = bs.readOctet();
			ending = bs.tell();

			if (bs.isEOF() || isspace(c)) {
				break;
			}
		}

		tok.size = ending - starting;

		return tok;
	}

	TriangleMesh * readASCIIFromStream(BinStream& bs)
	{
		char buff[256];
		size_t buffLen = 256;

		TriangleMesh* mesh = new TriangleMesh();
		// read first line
		// it should be 'solid' <name>
		bs.readLine(buff, buffLen);
		//printf("FIRST: %s\n", buff);

		BinStream ls(buff, strlen(buff));
		Token tok = nextToken(ls);
		//printf("tok: %p, %zd\n", tok.data, tok.size);
		//printf("  noun: %s\n", tok.toString());
		tok = nextToken(ls);
		//printf("  name: %s\n", tok.toString());

		while (!bs.isEOF()) {
			//   facet normal f1 f2 f3
			bs.readLine(buff, buffLen);
			BinStream ls(buff, strlen(buff));
			Token tok = nextToken(ls);	// facet
			//printf("facet: %s\n", tok.toString());
			if (tok.toString() == "endsolid")
				break;

			tok = nextToken(ls);		// normal

			vec3f n;
			tok = nextToken(ls);		// f1
			n[0] = tok.toFloat();
			tok = nextToken(ls);		// f2
			n[1] = tok.toFloat();
			tok = nextToken(ls);		// f3
			n[2] = tok.toFloat();

			// Add the normal to the mesh
			size_t nOffset = mesh->addNormal(n)-1;

			vec3i fNorms;
			fNorms[0] = nOffset;
			fNorms[1] = nOffset;
			fNorms[2] = nOffset;
			vec3i fVerts;
			
			bs.readLine(buff, buffLen);		// outer loop
			//printf("Outer Loop: %s\n", buff);

			for (int i = 0; i < 3; i++) {
				//       vertex f1 f2 f3
				bs.readLine(buff, buffLen);
				BinStream ls(buff, strlen(buff));
				Token tok = nextToken(ls);		// 'vertex'
				
				vec3f vert;
				tok = nextToken(ls);
				vert.x = tok.toFloat();
				tok = nextToken(ls);
				vert.y = tok.toFloat();
				tok = nextToken(ls);
				vert.z = tok.toFloat();

				size_t vOffset = mesh->addVertex(vert);
				fVerts[i] = vOffset;
			}
			mesh->addFace(fVerts, fNorms, {});	// no uv values
			bs.readLine(buff, buffLen);		//     endloop
			bs.readLine(buff, buffLen);		//     endfacet
		}

		return mesh;
	}

	TriangleMesh* readBinaryFromStream(BinStream& bs)
	{
		uint8_t buff[256];
		size_t buffLen=256;

		TriangleMesh* mesh = new TriangleMesh();

		// Read 80 by header
		bs.readBytes(buff, 80);

		// read little-endian, uint32, number of triangles
		uint32_t nTriangles = bs.readUInt32();
		//printf("BINARY: nTriangles: %d\n", nTriangles);
		for (size_t i = 0; i < nTriangles; i++)
		{
			// for each triangle
			// Face normal
			vec3f fn;
			fn.x = bs.readFloat();
			fn.y = bs.readFloat();
			fn.z = bs.readFloat();

			// Add the normal to the mesh
			size_t nOffset = mesh->addNormal(fn) - 1;
			//printf("NORMAL(%d): %f, %f, %f\n", nOffset, fn.x, fn.y, fn.z);

			vec3i fNorms;
			fNorms[0] = nOffset;
			fNorms[1] = nOffset;
			fNorms[2] = nOffset;

			vec3i fVerts;
			// read vertices
			for (size_t n = 0; n < 3; n++) {
				vec3f v;
				v.x = bs.readFloat();
				v.y = bs.readFloat();
				v.z = bs.readFloat();
				//printf("  vertex(%zd): %f, %f, %f\n", n, v.x, v.y, v.z);
				int vOffset = mesh->addVertex(v);
				fVerts[n] = vOffset;
			}
			uint16_t attr = bs.readUInt16();
			mesh->addFace(fVerts, fNorms, {});
		}
		
		return mesh;
	}

	TriangleMesh* readFromFile(const char* filename)
	{
		// Create a mmap
		auto bs = FileStream(filename);
		
		// Read first 80 bytes
		uint8_t buff[256];
		bs.readBytes(buff, 5);

		// if it starts with 'solid', then it's an ASCII
		// otherwise, it's binary
		bs.seek(0);
		TriangleMesh* mesh = nullptr;
		std::string sig((char*)buff, 5);
		//printf("sig: %s\n", sig);

		if (sig == "solid")
			mesh = readASCIIFromStream(bs);
		else
			mesh = readBinaryFromStream(bs);

		mesh->centerMesh();
		mesh->normalizeVertices();

		return mesh;
	}
}	// end of stl