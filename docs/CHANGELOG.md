# Open Edison 变更日志

> 从项目初始化到 v0.1.0 的完整提交记录。

---

## 2026-05-02

### 项目初始化

```
857efe5 chore: project skeleton with CMakeLists.txt and gitignore
```
- 创建项目目录，初始化 Git 仓库
- CMakeLists.txt 配置 JUCE 8 FetchContent
- .gitignore 配置

### P0 基础编辑器

```
88f78bb feat: P0 basic waveform editor skeleton
```
14 个源文件：
- **Main.cpp** — JUCE 应用入口
- **MainComponent** — 主窗口 + 菜单栏 + 命令路由
- **WaveformThumbnail** — 波形渲染 + 缩放
- **AudioFileManager** — WAV/MP3/FLAC 加载
- **TransportBar** — 播放/暂停/停止/录音，AudioSource 回调
- **SelectionOverlay** — 鼠标框选 + 边缘手柄
- **SelectionManager** — 选区状态管理
- **DragExport** — 选区拖拽导出 WAV 骨架

### P1 进阶功能

```
bd81f5a feat: P1 spectrogram view (STFT real-time analysis)
6cc9b64 feat: P1 undo/redo framework + silence selection
6049d7f feat: P1 selection processing - reverse/normalize/fade in/fade out
86caa8e feat: add Delete/Backspace keybindings for silence selection
```
- 基于 STFT 的频谱/声谱图实时分析
- 完整的 UndoManager + AudioModifyAction 撤销框架
- 选区处理：反向播放、归一化、淡入淡出、静音

### P2 增强功能

```
4b78580 feat: spectrogram playhead cursor with real-time playback follow
cf808a8 feat: P2 mouse wheel fine zoom and horizontal scroll
d4c2057 feat: P0 DragExport integration - Alt+drag selection to export WAV
fd029c0 docs: update project_state.md after DragExport integration
a1d7a29 feat: P2 marker/cue points - add marker at playhead (M), remove (Shift+M), navigate ([ and ])
d6c9c9d docs: update project_state.md after Marker/Cue Points integration
d8fd43e feat: P2 multi-region management (Edison block concept)
53f4e22 feat: P2 zoom-to-selection (Z key) + fit all (Ctrl+Shift+F)
cf2efca docs: update project_state.md after Zoom-to-Selection integration
```
- 频谱图播放头实时光标跟随
- 鼠标滚轮精确横向缩放 + 滚动
- DragExport Alt+拖拽选区导出 WAV 完成
- 标记/提示点系统（M/Shift+M/[ / ]）
- 多 Region 管理（Edison 的"块"概念）
- Z 缩放到选区，Ctrl+Shift+F 适应全部

### P3 高级功能

```
56cf48f feat: P3 loop/cycle playback (A-B repeat)
533988f feat: P3 zero-crossing snapping for selection edges (X key toggle)
0881369 docs: update project_state.md after zero-crossing snapping integration
d0a8e5e feat: P3 crossfade on selection silence (automatic 5ms fade at boundaries)
18c2b82 docs: update project_state.md after crossfade-on-silence integration
3b393e6 feat: P3 snap-to-grid (G key toggle, 120 BPM default, beat/bar aligned snapping)
7cf21ae docs: update project_state.md after snap-to-grid integration
```
- A-B 区间循环/周期播放
- 零交叉吸附（X 键开关）
- 静音时自动 5ms 交叉淡入淡出
- 吸附到网格（G 键开关，120 BPM，支持节拍/小节对齐）

### 文档

```
33194f7 docs: add README.md with full feature documentation and project overview
```

---

## 2026-05-11

### GitHub 推送

```
de14e19 docs: add one-click Windows setup (setup.ps1) with zero manual downloads
```
- 新增 `setup.ps1` Windows 一键安装脚本
- 支持 winget/MSYS2、Chocolatey、Scoop 自动检测
- 零手动网页浏览下载依赖
- README 增加 Windows 构建说明

---

## 2026-05-12

### 单元测试

```
（本次提交）
```
- 新增单元测试框架（JUCE UnitTest）
- 6 个测试模块，覆盖全部核心逻辑
- 独立的 `OpenEdison_Tests` 构建目标
- 完整的测试文档

### 项目文档

```
（本次提交）
```
- `docs/ARCHITECTURE.md` — 系统架构设计文档
- `docs/CHANGELOG.md` — 完整变更日志
- `docs/TESTING.md` — 测试说明文档
- `docs/DEVELOPMENT_LOG.md` — 开发过程记录
- `docs/README.md` — 文档索引
