# Open Edison 架构设计文档

> 版本：0.1.0 | 更新日期：2026-05-12

---

## 1. 项目概述

Open Edison 是一个基于 JUCE 8 的轻量级桌面音频波形编辑器，灵感源自 FL Studio 的 Edison。核心设计理念是**模块化**——每个核心概念作为一个独立的 Manager + Overlay/Component 对来实现。

---

## 2. 技术栈

| 层级 | 技术 | 说明 |
|------|------|------|
| 应用框架 | JUCE 8 | 音频、GUI、事件系统、跨平台抽象 |
| 构建系统 | CMake 3.21+ | FetchContent 自动拉取 JUCE |
| 语言标准 | C++20 | 智能指针、lambda、类型推导 |
| 测试框架 | JUCE UnitTest | 内置于 JUCE Core，无需额外依赖 |
| 跨平台 | Linux（开发）+ Windows（目标） | CMake 生成器切换 |

### 关键依赖关系

```
OpenEdison (可执行文件)
    ├── juce_core            (字符串、集合、线程、文件系统)
    ├── juce_events          (消息循环、定时器)
    ├── juce_graphics        (颜色、路径、图像)
    ├── juce_gui_basics      (组件、按钮、布局)
    ├── juce_gui_extra       (菜单栏、文档窗口)
    ├── juce_data_structures (UndoManager)
    ├── juce_audio_basics    (AudioBuffer、AudioSource)
    ├── juce_audio_devices   (音频 I/O 设备)
    ├── juce_audio_formats   (WAV/MP3/FLAC 读写)
    └── juce_audio_utils     (AudioThumbnail)
```

---

## 3. 核心模块架构

### 3.1 整体数据流

```
用户输入（鼠标/键盘）
    ↓
MainComponent（主路由）
  ├── 命令分发（ApplicationCommandManager）
  │     ↓
  ├── SelectionManager ←→ AudioFileManager
  │     ↓                    ↓
  │  SelectionOverlay    WaveformThumbnail
  │  (UI覆盖层)          SpectrogramComponent
  │
  ├── TransportBar（AudioSource 回调 → 音频设备）
  │     ↓
  │  AudioFileManager（缓冲区读写）
  │
  ├── DragExport（Alt+拖拽选区 → WAV 导出）
  ├── MarkerManager → MarkerOverlay
  ├── RegionManager → RegionOverlay
  ├── LoopManager → LoopOverlay
  └── GridManager → GridOverlay
```

### 3.2 各模块职责

| 模块 | 类名 | 职责 |
|------|------|------|
| **音频核心** | `AudioFileManager` | 加载/卸载音频文件、管理 AudioBuffer、AudioThumbnail、零交叉查找 |
| **波形渲染** | `WaveformThumbnail` | 基于 AudioThumbnail 的波形绘制、水平/垂直缩放 |
| **频谱图** | `SpectrogramComponent` | 基于 STFT 的频谱/声谱图实时分析渲染 |
| **播放引擎** | `TransportBar` | 继承 AudioSource，实现播放/暂停/停止/录音，进度条控制 |
| **选区管理** | `SelectionManager` | 选区起止时间、吸附逻辑（零交叉 + 网格） |
| **选区 UI** | `SelectionOverlay` | 鼠标拖拽创建选区、边缘手柄拖动、选区信息显示 |
| **选区导出** | `DragExport` | Alt+拖拽选区到文件管理器 → 自动导出 WAV |
| **撤销/重做** | `AudioModifyAction` | 继承 UndoableAction，保存原始采样数据用于撤销 |
| **标记系统** | `MarkerManager` | 添加/删除/导航标记点，标签生成 |
| **标记 UI** | `MarkerOverlay` | 标记在波形上的视觉标注 |
| **Region 管理** | `RegionManager` | 音频区域（块）的添加/删除/重命名/导航 |
| **Region UI** | `RegionOverlay` | Region 在波形上的色块标注 |
| **循环播放** | `LoopManager` | A-B 区间循环播放状态管理 |
| **循环 UI** | `LoopOverlay` | 循环区间视觉标注 |
| **网格系统** | `GridManager` | BPM/节拍计算、网格线生成、吸附算法 |
| **网格 UI** | `GridOverlay` | 网格线和节拍线绘制 |

### 3.3 模块间依赖关系

```
                          AudioFileManager
                               │
           ┌───────────────────┼──────────────────────┐
           ↓                   ↓                      ↓
    WaveformThumbnail   SpectrogramComponent    TransportBar
           ↓                                       ↓
    SelectionManager ←───────────────────────────┘
      │      │            │               │
      ↓      ↓            ↓               ↓
  DragExport GridManager  MarkerManager   LoopManager
      │                   │
      ↓                   ↓
  SelectionOverlay    MarkerOverlay
      │
      ↓
  RegionManager → RegionOverlay
```

---

## 4. 测试架构

### 4.1 测试模块

| 测试类 | 测试模块 | 测试类别 | 测试数量 |
|--------|---------|---------|---------|
| `AudioFileManagerTest` | AudioFileManager | EdisonCore | 2 组 |
| `GridManagerTest` | GridManager | EdisonCore | 10 组 |
| `LoopManagerTest` | LoopManager | EdisonCore | 8 组 |
| `MarkerManagerTest` | MarkerManager | EdisonCore | 8 组 |
| `RegionManagerTest` | RegionManager | EdisonCore | 10 组 |
| `SelectionManagerTest` | SelectionManager | EdisonCore | 9 组 |
| `SnapToZeroTest` | AudioFileManager::snapToZeroCrossing | EdisonAudio | 1 组 |

### 4.2 测试隔离

测试目标与主程序独立——`OpenEdison_Tests` 只链接非 GUI 的源文件（`AudioFileManager.cpp`、`RegionManager.cpp`），不依赖任何 UI 组件。

---

## 5. 音频流处理

### 5.1 播放路径

```
TransportBar::getNextAudioBlock(bufferToFill)
    ↓
从 AudioFileManager::getBuffer() 读取
    ↓
根据当前播放位置 + 选区状态决定是否播放
    ↓
填充 bufferToFill（JUCE 音频回调）
    ↓
音频设备播放
```

### 5.2 选区导出路径

```
用户 Alt+鼠标拖拽选区
    ↓
DragExport 创建临时 WAV 文件（16-bit 44.1kHz）
    ↓
performExternalDragDropOfFiles() 触发 OS 级拖拽
    ↓
用户松手 → 文件保存到目标位置
```

### 5.3 录音路径

```
TransportBar::startRecording()
    ↓
创建 AudioBuffer<float> 接收音频设备输入
    ↓
TransportBar::stopRecording()
    ↓
将 AudioBuffer 写为 WAV 文件
    ↓
回调 loadAudioFile() 自动加载到编辑器
```

---

## 6. 撤销/重做机制

```
AudioModifyAction（继承 juce::UndoableAction）
    │
    ├─ 构造时传入：AudioFileManager、startSample、numSamples、Processor 函数
    │
    ├─ perform() → 执行 Processor（静音/反向/归一化等）
    │              触发 onAudioChanged 通知波形重绘
    │
    ├─ undo()    → 从 m_originalSamples 恢复原始采样数据
    │              触发 onAudioChanged 通知波形重绘
    │
    └─ 通过 juce::UndoManager 管理栈
         Ctrl+Z = undo(), Ctrl+Shift+Z = redo()
```

---

## 7. 键盘快捷键系统

| 快捷键 | 功能 | 实现方式 |
|--------|------|---------|
| 空格 | 播放/暂停 | ApplicationCommandManager + KeyPressMapping |
| Ctrl+O | 打开 | FileChooser::launchAsync |
| Ctrl+Z / Ctrl+Shift+Z | 撤销/重做 | UndoManager |
| Delete/Backspace | 静音选区 | MainComponent 键盘事件 |
| Z | 缩放到选区 | WaveformThumbnail 缩放控制 |
| Ctrl+Shift+F | 适应全部 | 重置缩放 |
| X | 零交叉吸附开关 | SelectionManager 切换 |
| G | 网格吸附开关 | SelectionManager 切换 |
| M | 添加标记 | MarkerManager |
| Shift+M | 删除标记 | MarkerManager |
| [ / ] | 上/下一个标记 | MarkerManager 导航 |
| Alt+拖拽选区 | 导出 WAV | DragExport |

---

## 8. 构建系统

### 8.1 CMake 结构

```
CMakeLists.txt
    ├── FetchContent(JUCE 8.0.5)  # 自动拉取
    ├── add_executable(OpenEdison) # 主程序
    │     └── 链接 10 个 JUCE 模块
    ├── add_executable(OpenEdison_Tests) # 测试程序
    │     ├── Source/tests/*.cpp
    │     ├── Source/AudioFileManager.cpp
    │     └── Source/RegionManager.cpp
    └── 平台特定链接选项（Linux: -Wl,--no-undefined）
```

### 8.2 测试配置

测试目标的 CMake 定义：
```cmake
target_compile_definitions(OpenEdison_Tests PRIVATE
  JUCE_STANDALONE_APPLICATION=1
  JUCE_UNIT_TESTS=1
)
```

### 8.3 Windows 构建

通过 `setup.ps1` 脚本自动安装依赖：
1. 检测 winget/MSYS2、Chocolatey、Scoop
2. 安装 MinGW-w64 GCC + CMake + Ninja + Git
3. 零手动网页浏览

---

## 9. 文件结构

```
open-edison/
├── CMakeLists.txt                      # 构建系统
├── setup.ps1                           # Windows 一键安装
├── README.md                           # 项目 README
├── .gitignore                          # Git 忽略规则
├── Source/
│   ├── Main.cpp                        # JUCE 应用入口
│   ├── MainComponent.h/.cpp            # 主窗口 + 菜单 + 命令路由
│   ├── AudioFileManager.h/.cpp         # 音频文件管理
│   ├── WaveformThumbnail.h/.cpp        # 波形渲染
│   ├── SpectrogramComponent.h/.cpp     # 频谱图渲染（STFT）
│   ├── TransportBar.h/.cpp             # 播放引擎 + 录音
│   ├── SelectionManager.h              # 选区状态
│   ├── SelectionOverlay.h/.cpp         # 选区 UI
│   ├── DragExport.h/.cpp               # 选区拖拽导出
│   ├── UndoableActions.h/.cpp          # 撤销/重做
│   ├── MarkerManager.h                 # 标记数据
│   ├── MarkerOverlay.h/.cpp            # 标记 UI
│   ├── RegionManager.h/.cpp            # Region 管理
│   ├── RegionOverlay.h/.cpp            # Region UI
│   ├── LoopManager.h                   # 循环播放状态
│   ├── LoopOverlay.h/.cpp              # 循环 UI
│   ├── GridManager.h                   # 网格数据
│   ├── GridOverlay.h/.cpp              # 网格 UI
│   └── tests/
│       ├── TestMain.cpp                # 测试入口
│       ├── TestAudioFileManager.cpp    # AudioFileManager 测试
│       ├── TestGridManager.cpp         # GridManager 测试
│       ├── TestLoopManager.cpp         # LoopManager 测试
│       ├── TestMarkerManager.cpp       # MarkerManager 测试
│       ├── TestRegionManager.cpp       # RegionManager 测试
│       └── TestSelectionManager.cpp    # SelectionManager 测试
├── docs/
│   ├── README.md                       # 文档索引
│   ├── CHANGELOG.md                    # 变更日志
│   ├── ARCHITECTURE.md                 # 架构文档（本文档）
│   ├── TESTING.md                      # 测试文档
│   └── DEVELOPMENT_LOG.md              # 开发日志
└── project_state.md                    # 项目管理
```
