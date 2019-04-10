## Example 2


This example shows how to carry out the network analysis starting from p value diagrams by means of the netOnZeroDXC_analysis app.
Each p value diagram is stored is a file "pdiag_<i>_<j>.dat" in the folder "pdiagrams". There are 15 nodes, and therefore 105 diagrams.
This scenario is typically encountered when the assessmente of p value diagrams is carried out at a previous time or, for example, with the command line program netOnZeroDXC_diagram.

- Open the netOnZeroDXC_analysis app. At first, only the upper panel (Input settings) is enabled.

- Select "Diagrams of p-value: N(N-1)/2 files" as starting data, "tab" as a column separator and "underscore" as file label delimiter.

- Load all the "pdiag_<i>_<j>.dat" files present in the "pdiagrams" folder with the "Load files" button. The app gives a feedback on the loaded file, reporting on the number of diagrams detected and their size. At this stage, if the app detects inconsistencies or fails to read the input file(s), a meaningful error message is given.

- A set of other controls is enabled after a successful load. Set the folder where to save output data. You can choose any folder, provided the app has permissions to write in such folder.

- Set the target of the analysis to "Matrix of time scales" and tune the parameters in this way:
	Sampling period = 0.01
	Base width = 10 points
	Significance threshold = 0.05
	Efficiency threshold = 0.90
Notice that the "Sampling period" and "Base width" settings for this kind of loaded data (p value diagrams) have to be provided because, otherwise, the app cannot assign a window width to each diagram row. Indeed, window width information is not stored in p value diagrams. The significance threshold is set to 0.05 and the efficiency threshold is left to the default value of 50%. The effect of changing these last threshold parameters can be studied by means of the post-analysis preview. You can set a prefix for output files so that it is easier to identify them.
The analysis is then started. Parallel computing is only exploited by the previous step of the analysis (p value assessment), so in this case it is not relevant.

- When the analysis ends, the preview button allows to examine the matrix of time scales, and to see the effect of changing the two threshold parameters.


That's it! You can find the results (and any intermediate file saved) in the selected output folder. In this folder, you can find a picture of what the matrix should look like with the parameters set as above.
