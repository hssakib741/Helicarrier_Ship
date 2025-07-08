# 🛸 Helicarrier Scene in OpenGL

This C++ OpenGL project is a **dynamic 2D animated scene** featuring a **futuristic Helicarrier** floating over water with interactive **fighter jet landing**, **day/night transition**, **rotating searchlights**, and **helicopters**. Designed using **GLUT**, this project showcases beginner-to-intermediate level graphics programming with smooth animations and event handling.

---

## 🌟 Features

- ✈️ **Fighter Jet Animation**
  - Bézier-curve-based landing sequence
  - Rolling and parking after landing

- 🌞🌙 **Day & Night Cycle**
  - Smooth transition with fading sun/moon and stars
  - Realistic sky gradients

- 🌊 **Animated Environment**
  - Dynamic ocean waves
  - Moving clouds
  - Rotating searchlight beams

- 🚁 **Helicopter Display**
  - Multiple helicopters on the Helicarrier

- 🔦 **Lighting Effects**
  - Searchlights and glowing deck lights at night

- 🔊 **Background Sound**
  - Ambient ocean sound using `sndPlaySound()` (requires `ocean.wav` file)

---

## 📸 Screenshots

### 🌞 Day Mode
![Day Mode](Plane Landed.png)

### 🌙 Night Mode
![Night Mode](Night.png)

## 🧪 Controls

| Key | Action                          |
|-----|---------------------------------|
| `L` | Initiate fighter jet landing    |
| `N` | Switch to night mode            |
| `D` | Switch to day mode              |

---

## 🛠️ Technologies Used

- **Language:** C++
- **Graphics:** OpenGL, GLUT
- **Sound:** `mmsystem.h` (plays WAV sound)
- **Platform:** Windows

---

## 📁 File Structure

Helicarrier/
├── main.cpp # Full source code
├── ocean.wav # Sound file (needs to be placed in the working directory)
├── README.md # This file

yaml
Copy
Edit

---

## 🧰 Dependencies

- **OpenGL Utility Toolkit (GLUT)**
- **Windows Multimedia System (`mmsystem.h`)** for sound
- **g++ or MinGW compiler**

---

## ▶️ How to Run

### ✅ Compile:

```bash
g++ main.cpp -o Helicarrier -lglut32cu -lglu32 -lopengl32 -lwinmm
Ensure you have glut32.dll in your system or project folder.

▶️ Run:
bash
Copy
Edit
./Helicarrier
Place the ocean.wav file in the same directory as the executable for sound to work.

🎬 Screenshots
(Add screenshots here in your repo once available)

💡 Educational Value
This project is great for learning:

OpenGL rendering pipeline

Transformations and matrix manipulation

Bézier curve animations

Interactive keyboard event handling

Blending and transparency

Basic sound integration in C++

📄 License
MIT License — feel free to use, modify, or share this project!
