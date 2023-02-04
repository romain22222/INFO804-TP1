#ifndef UTILS_HEADER
#define UTILS_HEADER
#include <istream>
#include <ostream>
#include <vector>
#include <iostream>
#include <map>
#include "math.h"

// Utils.h
struct Vecteur {
	float xyz[ 3 ]; // les composantes
	Vecteur()
	: Vecteur(0,0,0) {}

	Vecteur( float x, float y, float z )
	:xyz{x,y,z} {}; // constructeur
	float operator[]( int i ) const {	// accesseur en lecture
		return xyz[i];
	}
	float& operator[]( int i ) { // accesseur en ecriture
		return xyz[i];
	}

	// Retourne le vecteur dont les composantes sont les minima des
	// composantes de soi-même et de other.
	Vecteur inf( const Vecteur& other ) const {
		return Vecteur(std::min(this->xyz[0], other.xyz[0]), std::min(this->xyz[1], other.xyz[1]), std::min(this->xyz[2], other.xyz[2]));
	};
	// Retourne le vecteur dont les composantes sont les maxima des
	// composantes de soi-même et de other.
	Vecteur sup( const Vecteur& other ) const {
		return Vecteur(std::max(this->xyz[0], other.xyz[0]), std::max(this->xyz[1], other.xyz[1]), std::max(this->xyz[2], other.xyz[2]));
	}

	Vecteur cross( const Vecteur& v ) const {
		return Vecteur(xyz[1]*v[2]-xyz[2]*v[1], xyz[2]*v[0]-xyz[0]*v[2], xyz[0]*v[1]-xyz[1]*v[0]);
	}

	Vecteur operator-(const Vecteur& other) const {
		return Vecteur(this->xyz[0]-other.xyz[0], this->xyz[1]-other.xyz[1], this->xyz[2]-other.xyz[2]);
	}

	double norm() const {
		return std::sqrt(this->xyz[0]*this->xyz[0]+this->xyz[1]*this->xyz[1]+this->xyz[2]*this->xyz[2]);
	}

	Vecteur operator/(double v) const {
		return Vecteur(this->xyz[0]/v,this->xyz[1]/v,this->xyz[2]/v);
	}

	Vecteur normalize() const {
		return *this / this->norm();
	}

	Vecteur operator+(const Vecteur& other) const {
		return Vecteur(this->xyz[0]+other.xyz[0], this->xyz[1]+other.xyz[1], this->xyz[2]+other.xyz[2]);
	}
};
std::ostream& operator<<( std::ostream& out, Vecteur v );
std::istream& operator>>( std::istream& in, Vecteur& v );

struct Triangle {
	Vecteur vertices[ 3 ]; // les sommets
	Triangle(Vecteur v1, Vecteur v2, Vecteur v3)
	: vertices{v1, v2, v3} {};
	Triangle()
	: Triangle(Vecteur(),Vecteur(),Vecteur()) {}
	Vecteur operator[]( int i ) const {	// accesseur en lecture
		return vertices[i];
	}
	Vecteur& operator[]( int i ) { // accesseur en ecriture
		return vertices[i];
	}

	Vecteur normal() const {
		auto v1 = vertices[1] - vertices[0];
		auto v2 = vertices[2] - vertices[0];
		return v1.cross(v2).normalize();
	}
};

std::ostream& operator<<( std::ostream& out, Triangle v );
std::istream& operator>>( std::istream& in, Triangle& v );

struct TriangleSoup {
	std::vector<Triangle> triangles; // les triangles
	TriangleSoup() {}
	void read( std::istream& in ) {
		Triangle t;
		char junk[INT16_MAX];
		while (!in.eof()) {
			while (in.peek() == '#') {
				in.getline(junk, INT16_MAX, '\n');
			}
			if (in.eof()) break;
			in >> t;
			triangles.push_back(t);
		}
		triangles.pop_back();
	}

	void write(std::ostream& out) {
		for (auto t: triangles) {
			out << t << "\n";
		}
	}

	void boundingBox(Vecteur& low, Vecteur& up) const {
		low = triangles[0][0];
		up = triangles[0][0];
		for (auto t: triangles) {
			for (int i = 0; i < 3; i++) {
				low = low.inf(t[i]);
				up = up.sup(t[i]);
			}
		}
	}
};

/// Définit un index sur 3 entiers. Toutes les opérations usuelles
/// sont surchargées (accès, comparaisons, égalité).
struct Index {
	int idx[ 3 ];
	Index() {}
	Index( int i0, int i1, int i2 )
	{
		idx[ 0 ] = i0;
		idx[ 1 ] = i1;
		idx[ 2 ] = i2;
	}
	Index( int indices[] )
	{
		idx[ 0 ] = indices[ 0 ];
		idx[ 1 ] = indices[ 1 ];
		idx[ 2 ] = indices[ 2 ];
	}
	int  operator[]( int i ) const { return idx[ i ]; }
	int& operator[]( int i )       { return idx[ i ]; }
	bool operator<( const Index& other ) const
	{
		return ( idx[ 0 ] < other.idx[ 0 ] )
			   || ( ( idx[ 0 ] == other.idx[ 0 ] )
				   && ( ( idx[ 1 ] < other.idx[ 1 ] )
					   || ( ( idx[ 1 ] == other.idx[ 1 ] )
						   && ( idx[ 2 ] < other.idx[ 2 ] ) ) ) );
	}

	bool operator==( const Index& other ) const {
		return !(*this < other || other < *this);
	}
};

std::ostream& operator<<( std::ostream& out, Index i );

// Structure pour calculer le barycentre d'un ensemble de points.
struct CellData {
	Vecteur acc;
	int nb;
	// Crée un accumulateur vide.
	CellData(): acc(), nb(0) {}
	// Ajoute le point v à l'accumulateur.
	void add( const Vecteur& v ) {
		acc = acc + v;
		nb++;
	}
	// Retourne le barycentre de tous les points ajoutés.
	Vecteur barycenter() const {
		return acc/nb;
	}
};

struct TriangleSoupZipper {

	const TriangleSoup& i;
	TriangleSoup& o;
	Index s;

	Vecteur bmin;
	Vecteur bmax;

	Vecteur bs;

	std::map<Index, CellData> index2data;

	// Construit le zipper avec une soupe de triangle en entrée \a
	// anInput, une soupe de triangle en sortie \a anOutput, et un index \a size
	// qui est le nombre de cellules de la boîte découpée selon les 3 directions.
	TriangleSoupZipper( const TriangleSoup& anInput,
					   TriangleSoup& anOutput,
					   Index size ): i(anInput), o(anOutput), s(size) {
		i.boundingBox(bmin, bmax);
		bs = (bmax-bmin);
		bs = Vecteur(bs[0]/s[0],bs[1]/s[1],bs[2]/s[2]);
	}

	/// @return l'index de la cellule dans laquelle tombe \a p.
	Index index( const Vecteur& p ) const {
		Vecteur newP = p - bmin;
		newP = Vecteur(newP[0] / bs[0], newP[1] / bs[1], newP[2] / bs[2]);
		return Index(newP[0],newP[1],newP[2]);
	}
	/// @return le centroïde de la cellule d'index \a idx (son "centre").
	Vecteur centroid( const Index& idx ) const {
		return Vecteur(bmin[0] + bs[0] * idx[0] + bs[0] / 2, bmin[1] + bs[1] * idx[1] + bs[1] / 2, bmin[2] + bs[2] * idx[2] + bs[2] / 2);
	}

	void zip() {
		o.triangles.clear();
		for (auto t: i.triangles) {
			std::vector<Index> thisTI;
			for (int i = 0; i < 3; i++) {
				thisTI.push_back(index(t[i]));
				index2data[index(t[i])].add(t[i]);
			}
			if (thisTI[0] == thisTI[1] || thisTI[0] == thisTI[2] || thisTI[1] == thisTI[2]) {
				continue;
			}
			o.triangles.push_back(Triangle(centroid(thisTI[0]),centroid(thisTI[1]),centroid(thisTI[2])));
		}
	}

	void smartZip() {
		index2data.clear();
		zip();
		for (auto& t: o.triangles) {
			for (int i = 0; i < 3; ++i) {
				t[i] = index2data[index(t[i])].barycenter();
			}
		}
	}
};


#endif