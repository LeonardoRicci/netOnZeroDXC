## Example 4


This example shows how to merge data from different recordings of the same system by means of the netOnZeroDXC_merge app. 
The dataset consists of matrices of time scales stored in this folder. There are 24 nodes, and the corresponding labels are stored in the "labels.txt" file.

- Open the netOnZeroDXC_merge app.

- Add a new system by means of the "Add system" button. Then, select "Matrix of time scales" in the input panel. Add recordings, by loading all matrices for the selected system. The filename of each loaded file is then used as the name of the recording.

- A label "dictionary" is stored in this folder ("labels.txt"). You can use this file as a template for other datasets. The label names are loaded by means of the "Load node labels" button. If labels are not provided, nodes will be automatically numbered with integer numbers.

- The "Validate" button starts a detailed check on the consistency of the dataset. The requirements are described in the user manual. If the dataset is valid, the merging panel becomes available while the upper panel becomes unavailable. Adding further data by re-enabling the top panel with the corresponding button will require to re-validate the dataset.

- Preview can be shown with the two preview buttons (because there is only one system, the behavior of the two buttons is identical). Because matrices were loaded, the Efficiency Threshold cannot be trimmed. Notice that moving the ranking control also updates the corresponding boxes in the app main window.

- Set the parameters to some value, and save the results. Again, because there is only one system, the two "save" buttons provide the same output file.


That's it! In this folder, you can find a picture of what the globally merged matrix should look like with an arbitrary choice of parameters (see the sliders values).
