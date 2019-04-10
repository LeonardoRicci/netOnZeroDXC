## Example 1


This example shows how to carry out the whole network analysis starting from raw time series by means of the netOnZeroDXC_analysis app.
The time series are stored in a single multi-column file, "sequences.dat". There are 15 sequences, with 500 points each.
Please note that this is a dummy dataset, aimed at providing a trial example to test the app.


- Open the netOnZeroDXC_analysis app. At first, only the upper panel (Input settings) is enabled.

- Select "Sequences, single file" as starting data, and "tab" as a column separator. You should also set the label delimiter according to the format you want for the output files. The label delimiter is not relevant for the single-file input mode, and neither does the column number.

- Load the "sequences.dat" file present in this folder with the "Load files" button. The app gives a feedback on the loaded file, reporting on the number of sequences detected and their length. At this stage, if the app detects inconsistencies or fails to read the input file(s), a meaningful error message is given.

- A set of other controls is enabled after a successful load. Set the folder where to save output data. You can choose any folder, provided the app has permissions to write in such folder.

- Set the target of the analysis to "Matrix of time scales" and tune the parameters in this way:
	Sampling period = 1.0
	Base width = 4 points
	Nr. of widths = 50
	Nr. of surrogates = 500
	Significance threshold = 0.05
	Efficiency threshold = 0.50
This settings allow to explore a range of time scales from 4.0 to 200.0 (in units of the sampling period). Since the significance threshold is set to 5%, the number of surrogates does not need to be too large. In particular, setting this last parameter to 500 corresponds to a resolution on p values of 0.2%. Finally, the efficiency threshold is left to the default value of 50%. The effect of changing this last parameter, as well as the significance threshold, can be studied by means of the post-analysis preview. In order to be able to change these parameters without re-running the whole analysis, the intermediate results in terms of p value diagrams must be saved. You can set a prefix for output files so that it is easier to identify them.
The analysis is then started. If possible, enabling parallel computing can accelerate the analysis.

- While the analysis runs, a progress dialog gives some information on which stage is being executed. It is also possible to stop the analysis by pressing the "Cancel" button. As you can notice, the longest stage to be run is the computation of p value diagrams by surrogate generation.

- When the analysis ends, the preview button allows to examine the matrix of time scales, and to see the effect of changing the two threshold parameters.


That's it! You can find the results (and any intermediate file saved) in the selected output folder. In this folder, you can find a picture of what the matrix should look like with the parameters set as above.
