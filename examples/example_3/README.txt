## Example 3


This example shows how to merge data from different recordings and systems by means of the netOnZeroDXC_merge app. In this example, a configuration file is used to load data. 
The dataset consists of efficiency functions stored under /data. There are 10 nodes, labelled with letters from "a" to "j". Consequently, the number of node pairs (and therefore of efficiency files per recording) is 45.

- Open the netOnZeroDXC_merge app.

- Select the "Configured load". Column separator and filename delimiter should be set to the default ones, tab and underscore. When the file selection dialog opens, select the file "config.cfg" in this folder. You can open the configuration file with any text editor, and examine its structure. This can be used as a template to create new configuration files.

- The app loads the data as described in the configuration file. System and recording names are those reported in the configuration. The "mode" label was set to "e", so the app expects to be fed with a list of names of efficiency files from which data must be fetched. Indeed, the fourth column of the configuration file is filled with the name of files that are found in the /data/ folder. Each file contains a list of efficiency files. Notice that all the paths are always given as relative to the folder in which the configuration file is stored. The app gives a feedback on the loaded dataset, reporting on the number of recordings/systems created. At this step, if the app detects inconsistencies or failures to read the input file(s), a meaningful error message is given. The button "Info on recording" shows a dialog with a brief description of the data stored for the selected recording.

- No other data are to be loaded, so the dataset can be validated and prepared to carry out the actual merging. The "Validate" button starts a detailed check on the consistency of the dataset. The requirements are described in the user manual. If the dataset is valid, the merging panel becomes available while the upper panel becomes unavailable. Adding further data by re-enabling the top panel with the corresponding button will require to re-validate the dataset.

- Preview can be shown either for a single, selected system (i.e. for the "within-system" merged matrix) or for the whole dataset ("between-systems" merged matrix). Because efficiencies were loaded, all controls including the Efficiency Threshold one are available. Notice that moving the controls also updates the corresponding boxes in the app main window.

- Set the parameters to some value, and save the results. It is possible to save either all "within-system" merged matrices (i.e. one per system), or just the global "between-systems" merged matrix. In the latter case, a standard dialog to save the file is shown, where the path and filename of the output can be arbitrarily set. In the former case, the app asks the user to select a folder where to save data. Then, it also asks for an optional prefix to be used to generate filenames.

That's it! In this folder, you can find a picture of what the globally merged matrix should look like with an arbitrary choice of parameters (see the sliders values).


- In this folder you also find a matrix version of the configuration file. In this case, the mode label is "m", and filenames correspond to matrix files. The procedure is identical to the one described above.
