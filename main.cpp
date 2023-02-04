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
	if (argc < 2) {
		std::cout << "Utilisation : ./viewer <NOM_FICHIER>\n";
		std::cout << "Utilisation : ./viewer <NOM_FICHIER> <NOM_FICHIER_SORTIE> <SUBDIVISION_X> <SUBDIVISION_Y> <SUBDIVISION_Z>\n";
		return 1;
	}

	// Read command lines arguments.
	QApplication application(argc,argv);
	// Instantiate the viewer.
	Viewer viewerI;
	Viewer viewerO;
	TriangleSoup tsZipped;

	ifstream input(argv[1]); // input est un flux en entrée.
	if ( ! input.good() ) std::cerr << "ERROR" << std::endl;
	TriangleSoup ts;
	ts.read(input);
	std::cout << "Nombre de triangles utilisés sur l'entrée : " << ts.triangles.size() << "\n";
	input.close(); // à la fin

	// Give a name
	viewerI.setWindowTitle(argv[1]);
	viewerI.ptrSoup = &ts;

	// Make the viewer window visible on screen.
	viewerI.show();

	if (argc == 6) {
		viewerO.setWindowTitle(argv[2]);

		ofstream output(argv[2]); // input est un flux en entrée.
		if ( ! output.good() ) std::cerr << "ERROR" << std::endl;

		TriangleSoupZipper tsz(ts, tsZipped, Index(stoi(argv[3]),stoi(argv[4]),stoi(argv[5])));
		tsz.smartZip();

		tsZipped.write(output);

		viewerO.ptrSoup = &tsZipped;
		viewerO.show();

		std::cout << "Nombre de triangles utilisés sur la sortie : " << tsZipped.triangles.size() << "\n";
		std::cout << "Taux de compression : " << (float(tsZipped.triangles.size())/float(ts.triangles.size())*100.) << "\n";
	}

	// Run main loop.
	application.exec();
	return 0;
}
