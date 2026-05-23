# Open Edison

> 轻量级桌面音频波形编辑器 — 灵感源自 FL Studio Edison，专注于便捷的选区操作与导出。

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus)
![JUCE 8](https://img.shields.io/badge/JUCE-8-8DC63F)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey)

---

## ✨ 功能特性

### 核心编辑（P0）
| 功能 | 说明 |
|------|------|
| **音频加载** | 拖放或 Ctrl+O 打开 WAV / MP3 / FLAC |
| **波形显示** | 高精度波形渲染，深色主题 |
| **播放控制** | 播放 / 暂停 / 停止 / 选区播放（空格键切换） |
| **选区操作** | 鼠标拖拽框选，拖动边缘调整选区范围 |
| **缩放** | 鼠标滚轮水平缩放，垂直振幅自适应 |
| **录音** | 支持选择输入设备，实时录音 |
| **选区导出 WAV** | Alt + 拖拽选区 → 直接导出 WAV 文件到操作系统 |

### 进阶功能（P1）
| 功能 | 说明 |
|------|------|
| **频谱视图** | 基于 STFT 的实时频谱/声谱图分析 |
| **撤销/重做** | Ctrl+Z / Ctrl+Shift+Z 全功能 UndoManager |
| **选区处理** | 反向播放 / 静音 / 归一化 / 淡入淡出 |
| **Delete 快捷操作** | 选中区域按 Delete/Backspace 涟漪删除（删除+拉移） |

### 增强功能（P2）
| 功能 | 说明 |
|------|------|
| **标记/提示点** | M 键添加标记，Shift+M 删除，[ / ] 前后导航 |
| **缩放适配** | Z 键缩放到选区，Ctrl+Shift+F 适应全部 |
| **鼠标滚轮精细缩放** | 精确到采样级别的横向缩放 |
| **播放头跟随** | 频谱图实时播放位置光标 |
| **多 Region 管理** | 类似 Edison 的"块"概念，管理多个音频片段 |

### 高级功能（P3）
| 功能 | 说明 |
|------|------|
| **循环播放** | A-B 区间重复播放 |
| **零交叉吸附** | X 键开关，选区边缘自动对齐到零交叉点 |
| **静音交叉淡入淡出** | 对选区执行静音时自动添加 5ms 交叉淡入淡出 |
| **吸附到网格** | G 键开关，按节拍/小节对齐选区 |

### 新增功能（P5）
| 功能 | 说明 |
|------|------|
| **涟漪删除（Ripple Delete）** | Delete/Backspace 删除选区并拉移后续音频 |
| **裁剪（Crop）** | Ctrl+T 保留选区内容，删除选区外所有音频 |
| **音量/增益控制** | TransportBar 增益滑块，实时调整回放音量 (0x–3x) |

---

## 🚀 快速开始

### 环境要求

- **CMake** 3.21+
- **C++20** 编译器（GCC 13+ / Clang 18+ / VS 2022）
- **JUCE 8**（通过 FetchContent 自动拉取）

### Linux

```bash
# 安装依赖
sudo apt install cmake g++ libasound2-dev libjack-jackd2-dev \
  libwebkit2gtk-4.1-dev libx11-dev libfreetype-dev \
  libxrandr-dev libxinerama-dev libxcursor-dev

# 构建
git clone https://github.com/your-username/open-edison.git
cd open-edison
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 运行
./build/OpenEdison
```

### Windows

推荐使用 **Visual Studio 2022 / MSVC** 作为 Windows 开发工具链。JUCE 会由 CMake 通过 `FetchContent` 自动拉取，不需要单独安装 JUCE。

```powershell
# 安装基础工具
winget install --id Git.Git -e
winget install --id Kitware.CMake -e
winget install --id Ninja-build.Ninja -e
```

通过 **Visual Studio Installer** 安装 **Visual Studio 2022** 或 **Build Tools for Visual Studio 2022**，勾选：

- **Desktop development with C++**
- **MSVC v143**
- **Windows 10/11 SDK**
- **C++ CMake tools for Windows**（使用 Visual Studio 开发时建议安装）

使用 Visual Studio 生成器构建：

```powershell
git clone https://github.com/your-username/open-edison.git
cd open-edison

cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

.\build\Release\OpenEdison.exe
```

也可以在 **Developer PowerShell for VS 2022** 中使用 Ninja 构建，适合 VS Code / 命令行开发：

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build

.\build\OpenEdison.exe
```

如果普通 PowerShell 提示找不到 `cl.exe`，请改用 **Developer PowerShell for VS 2022**，或先运行 VS 的开发环境初始化脚本。


---

## ⌨️ 快捷键

| 快捷键 | 功能 |
|--------|------|
| **空格** | 播放 / 暂停 |
| **Escape** | 停止播放，回到起点 |
| **Ctrl+O** | 打开音频文件 |
| **Ctrl+Shift+S** | 保存为 WAV (Save As) |
| **Ctrl+N** | 新建项目 |
| **Ctrl+Z** | 撤销 |
| **Ctrl+Shift+Z** | 重做 |
| **Ctrl+X** | 剪切选区 |
| **Ctrl+C** | 复制选区 |
| **Ctrl+V** | 粘贴剪贴板 |
| **Delete / Backspace** | 涟漪删除（删除选区并拉移后续音频） |
| **Ctrl+T** | 裁剪（保留选区，删除选区外音频） |
| **Ctrl+A** | 全选整个音频 |
| **Ctrl+D** | 静音选区 |
| **Delete / Backspace** | 静音选区 |
| **Ctrl+R** | 反向选区 |
| **Ctrl+Shift+N** | 归一化选区 |
| **Ctrl+I** | 选区淡入 |
| **Ctrl+Shift+O** | 选区淡出 |
| **Z** | 缩放到选区 |
| **Ctrl+Shift+F** | 适应全部波形 |
| **=** | 放大 |
| **-** | 缩小 |
| **Ctrl+S** | 切换波形/频谱视图 |
| **L** | 切换循环/AB 复读 |
| **X** | 切换零交叉吸附 |
| **G** | 切换网格吸附 |
| **M** | 添加标记/提示点 |
| **Shift+M** | 删除当前标记 |
| **[ / ]** | 上一个 / 下一个标记 |
| **Ctrl+Shift+R** | 从选区添加 Region |
| **Shift+Backspace** | 删除当前 Region |
| **Ctrl+Tab** | 下一个 Region |
| **Ctrl+Shift+Tab** | 上一个 Region |
| **Alt+拖拽** | 选区拖拽导出 WAV |

---

## 🏗️ 项目结构

```
open-edison/
├── CMakeLists.txt                     # CMake 构建（FetchContent JUCE 8）
├── Source/
│   ├── Main.cpp                       # JUCE 应用入口
│   ├── MainComponent.h/.cpp           # 主窗口 + 布局 + 菜单命令
│   ├── AudioFileManager.h/.cpp        # 音频格式读写 + AudioThumbnail
│   ├── WaveformThumbnail.h/.cpp       # 波形渲染（缩放/滚动）
│   ├── SpectrogramComponent.h/.cpp    # 频谱图渲染（STFT）
│   ├── TransportBar.h/.cpp            # 播放控制 + 录音
│   ├── SelectionManager.h             # 选区状态管理
│   ├── SelectionOverlay.h/.cpp        # 选区/UI 覆盖层
│   ├── DragExport.h/.cpp              # 选区拖拽导出 WAV
│   ├── UndoableActions.h/.cpp         # 撤销/重做动作
│   ├── MarkerManager.h                # 标记/提示点数据
│   ├── MarkerOverlay.h/.cpp           # 标记 UI 覆盖层
│   ├── RegionManager.h/.cpp           # 多 Region 管理
│   ├── RegionOverlay.h/.cpp           # Region UI 覆盖层
│   ├── LoopManager.h                  # A-B 循环播放
│   ├── LoopOverlay.h/.cpp             # 循环 UI 覆盖层
│   ├── GridManager.h                  # 网格/吸附数据
│   └── GridOverlay.h/.cpp             # 网格 UI 覆盖层
├── project_state.md                   # 项目管理文档
└── .gitignore
```

---

## 🧩 架构设计

```
用户输入（鼠标/键盘）
    ↓
MainComponent（命令路由）
    ├── SelectionManager → SelectionOverlay
    ├── AudioFileManager → WaveformThumbnail / SpectrogramComponent
    ├── TransportBar（AudioSource 回调 → 音频设备）
    ├── DragExport（Alt+拖拽 → WAV 文件）
    ├── MarkerManager → MarkerOverlay
    ├── RegionManager → RegionOverlay
    ├── LoopManager → LoopOverlay
    └── GridManager → GridOverlay
```

核心设计原则：
- **模块化**：每个核心概念一个独立 Manager + Overlay/Component 对
- **JUCE AudioSource**：播放引擎通过 `juce::AudioSource` 接口实现
- **应用内拖拽**：选区导出通过 `JUCE::DragAndDropContainer` 实现
- **撤销/重做**：所有可撤销操作封装为 `juce::UndoableAction` 子类

---

## 📄 许可证

本项目基于 **GPLv3** 许可证（与 JUCE 8 的 GPLv3 兼容）。
如需商业闭源使用，请购买 [JUCE 商业授权](https://juce.com/)。

---

## 🙏 致谢

- [JUCE Framework](https://github.com/juce-framework/JUCE) — 跨平台音频应用框架
- [FL Studio Edison](https://www.image-line.com/) — 灵感来源
- [OpenClaw](https://github.com/openclaw/openclaw) — 项目开发流程管理
