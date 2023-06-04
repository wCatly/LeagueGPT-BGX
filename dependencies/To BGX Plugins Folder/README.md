## Why PDB File included in here?
PDB files contain valuable debugging information that helps identify the exact source code locations where crashes occur. By including PDB files in the project distribution, we can facilitate more accurate crash reporting and analysis.

## Benefits of Including PDB Files
1. **Accurate Crash Reports**: PDB files enable users to generate detailed crash reports that include information about the specific line of code where the crash occurred, variable values, and call stack traces.

2. **Efficient Issue Investigation**: Developers and experienced users can utilize the PDB files to reproduce crashes locally and inspect the state of the program during the crash. This aids in understanding the root cause and facilitates faster issue investigation.

3. **Collaborative Debugging**: With access to PDB files, users can share crash reports and feedback with developers more effectively. This collaborative approach helps in resolving issues promptly and delivering improved software quality.

## How to Use PDB Files
If you encounter a crash while using our software, you can utilize the PDB files to generate more detailed crash reports. Here's how you can do it:

1. **Locate the Crash Logs**: When a crash occurs, look for crash logs or error messages that the software generates. These logs typically include details such as the crash location and stack trace.

2. **Collect PDB Files**: Ensure that you have the PDB files provided along with the software distribution. These files contain the necessary debugging information to analyze the crash.

3. **Generate Crash Reports**: Use the crash logs and the corresponding PDB files to generate crash reports. There are various tools available, such as WinDbg, Visual Studio Debugger, or specialized crash reporting tools, which can process crash logs and associate them with the correct PDB files.

4. **Share Crash Reports**: Share the generated crash reports with the software development team or experienced users. This collaboration helps in analyzing the crashes, identifying patterns, and resolving issues efficiently.

## Providing Feedback and Reporting Issues
We encourage users to share crash reports, provide feedback, and report any issues they encounter. By collaborating and leveraging the provided PDB files, we can improve the software's stability and address any potential problems more effectively.

To report issues or provide feedback, please follow the guidelines mentioned in our CONTRIBUTING.md file.
