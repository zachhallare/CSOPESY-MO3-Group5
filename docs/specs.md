# CSOPESY Semi-Major Output 1 — Specifications

**Prepared By:** Gregory Cu
**Created By:** Neil Patrick Del Gallego, PhD
**Updated as of:** Sept 4, 2026
**Submission type:** By group

---

## 1. General Instructions

An OS emulator needs a **command interpreter** and a **display output**.

Refer to a general Linux/Windows PowerShell/Windows Command Line as a strong reference for the design of your command-line interface (CLI).

---

## 2. Checklist of Requirements

The system must implement **ALL** of the following features properly.

### 2.1 OS Emulator Main Console

An "OS emulator" that accepts a command input with display output. On launch, it should show something like:

```
Welcome to CSOPESY!

Group developer:
De La Cruz, Juan
Santos, Alex

Version date:

Command>
```

It should also support displaying an ASCII-art style header/logo (e.g. a stylized "CSOPESY" banner) above the same developer/version info and prompt.

The main menu console must have the following features:
1. Text marquee or ASCII graphics marquee
2. Accept commands to change the behavior of the text marquee

### 2.2 Command Interpreter

The command interpreter for the OS emulator must accept the following commands from the main menu:

| Command | Description |
|---|---|
| `help` | Displays the commands and their description |
| `start_marquee` | Starts the marquee "animation" |
| `stop_marquee` | Stops the marquee "animation" |
| `set_text` | Accepts a text input and displays it as a marquee |
| `set_speed` | Sets the marquee animation refresh rate in milliseconds |

---

## 3. Assessment Method

- Assessed via a **black box quiz system** in a **time-pressure format**.
- This format is meant to minimize drastic changes or "hacking" the project to ensure test cases are met.
- During the quiz, you should **only modify parameters** — you may **no longer recompile** the project.
- Test cases, parameters, and instructions will be provided per question.
- You must submit a **video file (.mp4)** demonstrating your project per question.
- Some questions will additionally require a **PowerPoint presentation** explaining implementation details.

### Important Dates

| Week | Milestone |
|---|---|
| Week 3 | Project submission |

(See AnimoSpace for specific dates.)

---

## 4. Submission Details

In advance of the quiz, prepare the following:

### 4.1 SOURCE
- Contains your source code.
- Add a `README.txt` with:
  - Your name
  - Instructions on running your program
  - The entry class file where the `main` function is located
- Alternative: a GitHub link.

### 4.2 PPT — Technical Report
Must contain a technical report of your system covering:
- Command recognition
- Console UI implementation
- Command interpreter implementation
- Process representation
- Scheduler implementation
- Finding the balance between having a fluid refresh rate and polling rate:
  - Identify the recommended values for your current hardware
  - Identify the limits of your refresh and polling rate — where screen tearing and/or a noticeable delay in recognizing typed characters occurs

### 4.3 Video

- Must be **seamless and not cut**.
- Minimum quality: **480p**; maximum quality: **720p**.
- Ensure your IDE's font size is large enough to be readable on video.
- Maximum video size: **1 GB**.
- You are **not allowed** to access/modify your code while performing the test case.
- The video must always show you pressing Run/Debug from your IDE to initialize your program.
- Once running, you are **not allowed to exit** the program or attempt to modify your code.
- Not following upload instructions will **invalidate your submission**.
- Ensure your MP4 is **directly embedded in your PPTX** (this will be reflected in your total file size).

---

## 5. Grading Scheme

Evidence for each test case must be recorded via video. Each test case has points allocated, graded as follows:

| Functional — No points | Functional — Partial points | Functional — Full points |
|---|---|---|
| The CLI did not pass the test case. **NO WORKAROUND** is available to produce the expected output. | The CLI did not pass the test case. **A workaround** is available to produce the expected output. | The CLI passed the test case using varying inputs and produced the expected output. |

---

## 6. Reference Material (from prompt)

- Sample SLURM batch script (`pytest.sh`) illustrating shebang, `#SBATCH` directives (job name, output file, mail notifications, error file), module loading, and running a Python executable — included as an example of a command-line/script interface style reference.
- Sample PowerShell session showing `$PSVersionTable`, and defining/calling a simple function (`SayHello`) — included as another CLI reference example.