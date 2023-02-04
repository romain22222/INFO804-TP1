#include <qapplication.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Viewer.h"
#include "Utils.h"

using namespace std;
int main(int argc, char** argv)
{
	// Read command lines arguments.
	QApplication application(argc,argv);
	// Instantiate the viewer.
	Viewer viewer;

	ifstream input( argc > 1 ? argv[1] : "tref.tri" ); // input est un flux en entrée.
	if ( ! input.good() ) std::cerr << "ERROR" << std::endl;
	TriangleSoup ts;
	ts.read(input);
	std::cout << ts.triangles.size() << "\n";
	input.close(); // à la fin

	// Give a name
	viewer.setWindowTitle(argc > 1 ? argv[1] : "tref.tri");

	TriangleSoup tsZipped;
	TriangleSoupZipper tsz(ts, tsZipped, Index(50,50,50));
	tsz.zip();

	std::cout << tsZipped.triangles.size() << "\n";
	viewer.ptrSoup = &tsZipped;


	// Make the viewer window visible on screen.
	viewer.show();
	// Run main loop.
	application.exec();
	return 0;
}
