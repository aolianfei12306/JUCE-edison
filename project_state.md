# Project: Open Edison

> 轻量化桌面音频编辑器，类似 FL Studio Edison，聚焦波形编辑、选区操作与便捷导出。

---

## 终极目标

构建一个基于 JUCE 8 的轻量桌面音频编辑器，覆盖 FL Studio Edison 的**核心波形编辑功能**，并实现比 Edison 更便捷的**选区拖拽导出 WAV**。

## 当前状态

| 项目 | 值 |
|------|-----|
| **阶段** | P0 Pending: DragExport 集成完成 |
| **完成度** | ~35% (P0: 100%, P1: 100%, P2: 50%) |
| **最后更新** | 2026-05-02 18:51 CST |
| **技术栈** | JUCE 8 + C++20 + CMake |
| **目标平台** | Windows（优先）/ Linux |

## 进度记录

| 日期 | 阶段 | 完成内容 | 状态 |
|------|------|---------|------|
| 2026-05-02 | P0 构建 | JUCE 8 构建系统 + 基础编辑器骨架（14 个源文件，编译通过） | ✅ |
| 2026-05-02 | P1 频谱 | 频谱/声谱图视图（STFT 渲染、波形/频谱切换、Ctrl+S 快捷键） | ✅ |
| 2026-05-02 | P1 处理 | Undo/Redo 框架（AudioModifyAction）+ Silence Selection（Ctrl+D），绑定 Ctrl+Z / Ctrl+Shift+Z | ✅ |
| 2026-05-02 | P1 处理 | Reverse / Normalize / Fade In / Fade Out 选区处理（Ctrl+R/Ctrl+N/Ctrl+I/Ctrl+O）| ✅ |
| 2026-05-02 | P2 快捷键 | Delete/Backspace 静音 + 确认 Space 播放/暂停快捷键已绑定 | ✅ |
| 2026-05-02 | P2 滚轮 | 鼠标滚轮精细缩放——Ctrl+滚轮缩放（因子1.05），滚轮水平滚动，Waveform/Spectrogram 双视图 | ✅ |
| 2026-05-02 | P0 导出 | DragExport 集成——Alt+选区内拖动导出 WAV（修复 44100 硬编码采样率 Bug），构建通过 | ✅ |

## 版本里程碑

### P0 — 可用的基础音频编辑器
- [x] CMake + JUCE 8 构建系统
- [x] 加载 WAV/MP3/FLAC + 波形渲染显示（AudioFileManager + WaveformThumbnail）
- [x] 播放/暂停/停止/选区播放（TransportBar 框架）
- [x] 鼠标框选选区 + 拖动调整边界（SelectionOverlay）
- [x] 选区拖拽导出 WAV（Alt+选区内拖动导出，修复 44100 硬编码 Bug）
- [x] 水平缩放 + 垂直缩放（WaveformThumbnail）
- [x] 录音（TransportBar 录音回调框架）

### P1 — 接近 Edison 核心体验
- [x] 频谱/声谱图视图（STFT 实时分析，预计算渲染）
- [x] 撤销/重做（UndoManager + AudioModifyAction 泛型框架）
- [x] 选区内处理：静音（Silence Selection, Ctrl+D）
- [x] 选区内处理：反向 / 归一化 / 淡入淡出

### P2 — 增强功能（按需）
- [ ] 多 Region 管理（Edison 的"块"概念）
- [x] 鼠标滚轮精细缩放（Ctrl+滚轮缩放，滚轮水平滚动），Waveform & Spectrogram 双视图
- [ ] 标记/提示点
- [x] 快捷键绑定（空格播放/暂停，Delete/Backspace 静音）

## 已知 Bug

（暂无）

## 待处理

1. **P0 DragExport** — 骨架完成，需在 Windows 实际测试拖拽交互（当前 Linux 环境无法验证）。
2. **频谱视图优化** — 长音频文件（>10 min）的 STFT 内存上限需优化；实时播放跟随渲染待实现。
3. **选区处理** — 全部实现：Silence / Reverse / Normalize / Fade In-Out。✅
4. **快捷键表** — 空格播放/暂停，Delete/Backspace 静音，其余已绑定。系结到 README 的 Key Bindings 段落。✅

## 架构设计

```
open-edison/
├── CMakeLists.txt          # 根 CMake（FetchContent JUCE）
├── Source/
│   ├── Main.cpp            # JUCE 入口
│   ├── MainComponent.h/cpp # 主窗口 + 波形显示 + 控制栏 + 命令处理
│   ├── AudioFileManager.h/cpp    # 音频文件管理
│   ├── SelectionManager.h        # 选区状态管理
│   ├── SelectionOverlay.h/cpp    # 选区叠加层渲染
│   ├── TransportBar.h/cpp        # 播放控制条
│   ├── WaveformThumbnail.h/cpp   # 波形缩略图渲染
│   ├── DragExport.h/cpp          # 拖拽导出核心
│   ├── SpectrogramComponent.h/cpp # 频谱/声谱图 STFT 渲染
│   ├── UndoableActions.h/cpp     # Undo/Redo 动作（AudioModifyAction）
├── .gitignore
├── project_state.md
└── README.md
```
