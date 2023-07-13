// Include standard c++ libraries used.
#include <iostream>
#include <string>

// Include necessary QT parts to work with input arguments and the file system.
#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>
#include <QDir>

// Include necessary parts of the OpenOrienteering Mapper project.
#include <core/map.h>
#include <core/map_view.h>
#include <core/objects/object.h>
#include <tools/cutout_tool.h>
#include <fileformats/xml_file_format_p.h>
#include <fileformats/ocd_file_export.h>

// Use namespaces to avoid having to type them out every time.
using namespace std;
using namespace OpenOrienteering;

// This is the main function, from which the program starts executing.
// It first gets the arguments using QT's QCommandLineParser, then grabs the source map from the provided path and then 
// loops over each of the layout files in the provided layout directory to combine it with the source map, perform cutout 
// and other actions, then exports the map as XML, OCAD and PDF files.
int main(int argc, char **argv) {
  // Define constants
  const QString version = "0.1.0";
  const QString borderSymbol = "900.0";
  const QString frameSymbol = "900.1";

  // First set an environment variable to make it work in CLI only despite OO Mapper using QT for GUI stuff.
  putenv("QT_QPA_PLATFORM=offscreen"); 

  // Set up the QT app to use QT and to make the Map class work.
  QApplication app(argc, argv);
  app.setApplicationName("layomap");
  app.setApplicationDisplayName("Layomap");
  app.setApplicationVersion(version);

  // Define parser to parse the arguments.
  QCommandLineParser parser;
  parser.setApplicationDescription("Layomap: A program to automatically apply layout files to orienteering maps.");
  parser.addHelpOption(); // Adds help option with -h or --help.
  parser.addVersionOption(); // Adds version option with -v or --version
  
  // Defining the arguments when running the application.
  const QCommandLineOption sourceOption(QStringList() << "s" << "source",
                                        QCoreApplication::translate("main", "Path to source file in XML (.omap/.xmap) format. If not provided, the program will look for sourcemap files in this order:\n'sourcemap.xmap', 'sourcemap.omap'"),
                                        QCoreApplication::translate("main", "source"));
  parser.addOption(sourceOption);
  
  const QCommandLineOption layoutsOption(QStringList() << "l" << "layouts",
                                        QCoreApplication::translate("main", "Path to layouts directory. Defaults to 'layouts'."),
                                        QCoreApplication::translate("main", "layouts"),
                                        "./layouts"); // Default value
  parser.addOption(layoutsOption);
  
  const QCommandLineOption outputOption(QStringList() << "o" << "output",
                                        QCoreApplication::translate("main", "Path to output directory. Defaults to 'output'."),
                                        QCoreApplication::translate("main", "output"),
                                        "./output"); // Default value
  parser.addOption(outputOption);

  // Process the actual command line arguments given by the user.
  parser.process(app);
  
  // Store source, layout and output in variables.
  QString source = parser.value(sourceOption);
  QString layouts = parser.value(layoutsOption);
  QString output = parser.value(outputOption);

  // If no source is provided, look for predefined source map files in current directory. If none is found, exit with error and show help.
  if (source == "") {
    cout << "No path to a source map file was provided";

    // Define array of source files to look for, in that order.
    const QString sourcefiles[] = {"sourcemap.xmap", "sourcemap.omap"}; 

    // Loop over the source maps and look for the files, set the source to the first file found and then break out of the for loop.
    for (QString sourcefile : sourcefiles) {
      if (QFile::exists(sourcefile)) {
        cout << ". Using '" << sourcefile.toStdString() << "' as source map file.\n";
        source = sourcefile;
        break;
      }
    }

    // If no source file was found, exit with error and show help.
    if (source == "") {
      cerr << ", and no default 'sourcemap' could be found. You must provide one with --source (-s) or provide a default 'sourcemap' file in this directory.\n";
      parser.showHelp(1);
    }
  }

  // Check if the provided source map file exists. If not, exit with error and show help.
  if (!QFile::exists(source)) {
    cerr << "ERROR: The provided source map file '" << source.toStdString() << "' does not exist.\n";
    parser.showHelp(1);
  }

  // Get the directory containing layout files and log an error if the directory is invalid, then show help.
  QDir layoutDir(layouts);
  if (!layoutDir.exists()) {
    cerr << "ERROR: The layout directory, '" << layouts.toStdString() << "', you provided (or the default directory) does not exist.\n";
    parser.showHelp(1);
  }

  // Create output dir if it does not exist.
  QDir outDir(output);
  if (!outDir.exists()) {
    if (!outDir.mkpath(".")) {
      cerr << "ERROR: Failed to create output directory '" << output.toStdString() << "'.\n";
      return 1; // Exit if it fails to create the directory.
    }
  }

  // Define Map object containing the source map in XML (omap/xmap) format and a MapView with default settings.
  Map srcMap;
  MapView srcView(&srcMap);

  // Define importer to import the map from source path.
  XMLFileImporter srcImporter(source, &srcMap, &srcView);

  // Import the map from XML file into srcMap object.
  srcImporter.doImport();

  // This is used to filter for files ending in extentions of ".layout.omap" and ".layout.xmap".
  layoutDir.setNameFilters(QStringList() << "*.layout.omap" << "*.layout.xmap"); 

  // Using QFileInfoList to get list of files.
  QFileInfoList list = layoutDir.entryInfoList();

  // Return an error if the directory contains no layout files.
  if (list.size() == 0) {
    cerr << "ERROR: There are no layout files in the provided layout directory. \nLayout files must end in .layout.omap or .layout.xmap.";
    parser.showHelp(1);
  }

  // Loop over the layout files, perform import and operations, and then and create and output subdir with exported files for each of them.
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);

    // Define a layout map and empty view to load layout into.
    Map layoutMap;
    MapView layoutView(&layoutMap);

    // Define importer for the layout map in this iteration.
    XMLFileImporter layoutImporter(
      fileInfo.absoluteFilePath(), 
      &layoutMap, 
      &layoutView
    );

    // Do the import of the layout map file.
    layoutImporter.doImport();

    // Get the name of the output subdirectory and exported files.
    const QString outName = fileInfo.baseName();

    // Prepare to store references to layout objects.
    PathObject* borderObject = nullptr;
    PathObject* frameObject = nullptr;

    // For all objects in layout map, check if it is a special layout object and store it as such.
    layoutMap.applyOnAllObjects([&](Object* object) {
      // If it is a border object (900.0).
      if (object->getSymbol()->getNumberAsString() == borderSymbol) {
        if (borderObject != nullptr) {
          cerr << "ERROR: Layout map '" << outName.toStdString() << "' contains more than one border object (Object with symbol 900.0.0).";
          parser.showHelp(1);
        }
        borderObject = object->asPath();
      }

      // If it is a frame object (900.1).
      if (object->getSymbol()->getNumberAsString() == frameSymbol) {
        if (frameObject != nullptr) {
          cerr << "ERROR: Layout map '" << outName.toStdString() << "' contains more than one frame object (Object with symbol 900.1.0)";
          parser.showHelp(1);
        }
        frameObject = object->asPath();
      }
    });

    // Import the source map into the layout map to combine them.
    layoutMap.importMap(srcMap, Map::ObjectImport); 

    // If the border exists, perform the cutout, then delete the border.
    if (borderObject != nullptr) {
      CutoutTool::apply(&layoutMap, borderObject, false);
      layoutMap.deleteObject(borderObject);
    }

    // Clear the selection to cutout everything outside the frame, including layout objects.
    layoutMap.clearObjectSelection(true);

    // If frame exists, perform cutout.
    if (frameObject != nullptr) {
      CutoutTool::apply(&layoutMap, frameObject, false);
    }

    // Make the subdirectory
    if(!outDir.mkdir(outName)) {
      // If it already exists, cd into it, and clear it.
      QDir subdir = outDir;
      subdir.cd(outName);
      subdir.removeRecursively();
      outDir.mkdir(outName);
    }

    // Define exporters to export the combined map as XML (.omap) and as OCAD (.ocd) format.
    XMLFileExporter omapExporter(outDir.filePath(outName + "/" + outName + ".omap"), &layoutMap, &layoutView);
    OcdFileExport ocad08Exporter(outDir.filePath(outName + "/" + outName + ".8.ocd"), &layoutMap, &layoutView, 8);
    OcdFileExport ocad09Exporter(outDir.filePath(outName + "/" + outName + ".9.ocd"), &layoutMap, &layoutView, 9);
    OcdFileExport ocad10Exporter(outDir.filePath(outName + "/" + outName + ".10.ocd"), &layoutMap, &layoutView, 10);
    OcdFileExport ocad11Exporter(outDir.filePath(outName + "/" + outName + ".11.ocd"), &layoutMap, &layoutView, 11);
    OcdFileExport ocad12Exporter(outDir.filePath(outName + "/" + outName + ".12.ocd"), &layoutMap, &layoutView, 12);

    // Export the files to the specified path.
    omapExporter.doExport();
    ocad08Exporter.doExport();
    ocad09Exporter.doExport();
    ocad10Exporter.doExport();
    ocad11Exporter.doExport();
    ocad12Exporter.doExport();

    cout << "Exported '" << outName.toStdString() << "' files.\n";
  }

  cout << "Successfully applied " << list.size() << " layout(s) and exported all map files.\n";
  return 0;
}