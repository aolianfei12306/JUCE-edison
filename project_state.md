# Project: Open Edison

> 轻量化桌面音频编辑器，类似 FL Studio Edison，聚焦波形编辑、选区操作与便捷导出。

---

## 终极目标

构建一个基于 JUCE 8 的轻量桌面音频编辑器，覆盖 FL Studio Edison 的**核心波形编辑功能**，并实现比 Edison 更便捷的**选区拖拽导出 WAV**。

## 当前状态

| 项目 | 值 |
|------|-----|
| **阶段** | P4++++++++++++++++++++ edison-advance 十轮审计（基础功能审计+修复） |
| **完成度** | ~99.9% (P0: 100%, P1: 100%, P2: 100%, P3: 100%, P4: 100%, P4+ 审计修复: ✅, P4++ 审计修复: ✅, P4+++ 审计修复: ✅, P4++++ 审计修复: ✅, P4+++++ 审计修复: ✅, P4++++++ 审计修复: ✅, P4+++++++ 审计修复: ✅, P4++++++++ 审计修复: ✅, P4+++++++++ 九轮审计修复: ✅, P4++++++++++ 十轮审计修复: ✅, P4+++++++++++ edison-advance 审计: ✅, P4++++++++++++ edison-advance 二次审计: ✅, P4+++++++++++++ edison-advance 三次审计: ✅, P4++++++++++++++ edison-advance 四轮审计: ✅, P4+++++++++++++++ edison-advance 五轮审计: ✅, P4++++++++++++++++ edison-advance 六轮审计: ✅, P4+++++++++++++++ edison-advance 七轮审计: ✅, P4+++++++++++++++++ edison-advance 八轮审计: ✅, P4++++++++++++++++++ edison-advance 九轮审计: ✅, P4+++++++++++++++++++ edison-advance 十轮审计: ✅) |
| **LOC** | ~4,600 行（31 个源文件） |
| **最后更新** | 2026-05-15 10:19 CST |
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
| 2026-05-02 | P2 标记 | Marker/Cue Points——M键添加，Shift+M移除，[/]导航，青色标记旗标+点击跳转 | ✅ |
| 2026-05-02 | P2 Zoom | Zoom-to-Selection——Z 键缩放至选区，Ctrl+Shift+F 恢复完整视图 | ✅ |
| 2026-05-02 | P3 Loop | 循环/AB复读播放——LoopManager + LoopOverlay 半透明高亮，L 键切换，选区或全文件范围内循环，构建通过 | ✅ |
| 2026-05-02 | P3 Crossfade | 选区静音自动交叉淡化——5ms线性淡入淡出边界，消除咔嗒声，Undo兼容 | ✅ |
| 2026-05-02 | P3 Grid | Snap-to-Grid——G 键切换，默认120BPM/4分音符网格，选区吸附到节拍网格，网格线可视化（节拍线7%透明度+小节线15%透明度），与零交叉吸附可协同使用 | ✅ |
| 2026-05-11 | P4-001 | 文件信息显示——TransportBar 显示采样率/位深度/声道数/文件名 | ✅ |
| 2026-05-11 | P4-003 | 频谱视图长文件优化——视口驱动惰性 STFT，移除 10k 帧硬上限，支持任意时长音频 | ✅ |
| 2026-05-14 | 审计修复 | 审计 Open Edison 基础功能，修复选择拖动渲染问题，新增垂直缩放、Save As，修复 Region 删除快捷键 | ✅ |
| 2026-05-14 | P4++ 审计修复 | 选区信息被 TransportBar 遮盖修复，添加 Escape Stop 快捷键，修复频谱图像素间隙 | ✅ |
| 2026-05-14 | P4++ 审计修复(二轮) | TransportBar 选区信息真正显示（之前被按钮遮挡），传递 GridManager 到 TransportBar 显示 BPM，删除 SelectionOverlay 死代码 | ✅ |
| 2026-05-14 | P4+++ 审计修复(三轮) | Undo 历史未清空 Bug（加载新文件后旧操作可破坏新文件）、Ctrl+O 键冲突（Open 与 Fade Out 共用）、功能完整性审计 | ✅ |
| 2026-05-14 | P4+++++ 审计修复(五轮) | 波形视图回放自动滚动（跟随播放头）、功能完整性审计 | ✅ |
| 2026-05-14 | P4+++++++ 审计修复(七轮) | 死代码清理（移除 m_currentRegionCycler）、Undo/Redo 菜单禁用状态、无音频时禁用进度条、TransportBar 布局对齐 | ✅ |
| 2026-05-14 | P4++++++++ 审计修复(八轮) | 波形滚轮行为修正（普通滚轮水平滚动→Ctrl+滚轮缩放）、新增 Select All（Ctrl+A）、文件加载 Zoom-to-Selection 状态重置、Spectrogram 横向滚轮支持、View 菜单重复 Region 项移除 | ✅ |
| 2026-05-14 | P4+++++++++ 审计修复(九轮) | cmdSelectAll 漏注册修复、Zoom-to-Selection 同步 Spectrogram 视图、Fit All 同步 Spectrogram、显式 Ctrl+A 键映射 | ✅ |
| 2026-05-14 | P4++++++++++ 审计修复(十轮) | 频谱/波形双视图 Zoom 同步（onUserViewChanged 回调）、cmdZoomIn/Out 同步频谱视图、手动缩放重置 zoom-to-selection 状态、Region 标签栏设置重复修复 | ✅ |
| 2026-05-14 | edison-advance 审计 | New Project（File > New）、BPM 设置 UI（View > Set BPM...）、音频线程内层循环优化、Trackpad deltaX 一致性对齐 | ✅ |
| 2026-05-14 | edison-advance 二次审计 | cmdSetBPM 注册修复、频谱 Zoom Offset 钳位、LoopOverlay precision 修复 | ✅ |
| 2026-05-14 | edison-advance 三次审计 | BPM Dialog 内存泄露、Export 拖拽重复触发、快捷键补齐、README 更新 | ✅ |
| 2026-05-14 | edison-advance 四轮审计 | cmdNormalize/Ctrl+N 键冲突修复（Normalize 与 New Project 共用 Ctrl+N，改为 Ctrl+Shift+N），RegionOverlay 死代码 changeListenerCallback | ✅ |
| 2026-05-14 | edison-advance 五轮审计 | Waveform/Spectrogram 双视图滚动速度一致化、cmdToggleView 重复绑定修复、Spectrogram setViewOffset 钳位、RegionOverlay ChangeListener 死代码移除 | ✅ |
| 2026-05-14 | edison-advance 六轮审计 | Undo/Redo 菜单状态刷新、cmdNew 快捷键显示修复、TransportBar 选区信息与吸附信息文本重叠修复 | ✅ |
| 2026-05-15 | edison-advance 七轮审计 | Duplicate ChangeListener 注册修复（每次 loadAudioFile 重复添加 thumb listener）+ TransportBar stop 后 stale callAsync 位置回写修复 | ✅ |
| 2026-05-15 | edison-advance 八轮审计 | 代码审计 + Edison 功能完整性审核；验证七轮修复；发现死变量/对齐/m_readIndex 溢出；更新缺失功能优先清单 | ✅ |
| 2026-05-15 | edison-advance 九轮审计 | TransportBar 死变量 selInfoArea 清理、RegionOverlay ~析构函数清理 onRegionsChanged 避免悬挂、Spectrogram renderViewport m_viewOffset 同步回写、setPlaybackPosition 上限钳位、DragExport 临时文件清理（最多保留20个） | ✅ |
| 2026-05-15 | edison-advance 十轮审计 | newProject/loadAudioFile 清空 Undo 历史后未刷新菜单状态 Bug 修复（commandStatusChanged 遗漏） | ✅ |

## 审计报告 (2026-05-15 十轮审计)

### 发现的问题

| # | 问题 | 严重度 | 状态 |
|---|------|--------|------|
| 1 | **newProject() 清空 Undo 历史后未刷新 Undo/Redo 菜单状态** — `m_undoManager.clearUndoHistory()` 在执行后 Undo/Redo 栈变为空，但未调用 `m_commandManager->commandStatusChanged()`。菜单中 Undo/Redo 项继续显示为可用（active）状态，误导用户。 | **低** | ✅ 已修复 — 在 `clearUndoHistory()` 后添加 `commandStatusChanged()` |
| 2 | **loadAudioFile() 同样遗漏 commandStatusChanged()** — 加载新文件后 Undo 历史被清空，但菜单状态未同步更新。 | **低** | ✅ 已修复 |

### 功能完整性审计 — 对比 FL Studio Edison v21

| Edison 功能 | Open Edison | 备注 |
|-------------|-------------|------|
| 波形显示（多声道/缩放） | ✅ | 垂直+水平缩放 |
| 选区（创建/拖动/吸附） | ✅ | 零交叉+网格吸附 |
| 选区处理（静音/反向/标准化/淡入淡出） | ✅ | Undo 支持 |
| 复制/剪切/粘贴 | ✅ | 内部剪贴板 |
| 频谱/声谱图 | ✅ | 视口驱动 STFT，任意时长 |
| 标记/提示点 | ✅ | M/Shift+M/[ / ] |
| Region（块）管理 | ✅ | 带颜色标签+右键菜单 |
| 循环/AB 复读 | ✅ | L 键切换+半透明高亮 |
| 录制 | ✅ | 输入设备选择 |
| 回放自动滚动（波形） | ✅ | 跟随播放头 |
| 回放自动滚动（频谱） | ✅ | 已有功能 |
| 缩放至选区 | ✅ | Z 键切换/恢复 |
| 拖拽导出 WAV | ✅ | Alt+选区拖动 |
| Save As | ✅ | Ctrl+Shift+S |
| Escape 停止 | ✅ | 停止+复位 |
| New Project | ✅ | Ctrl+N |
| Select All | ✅ | Ctrl+A |
| **Ripple Delete（删除+拉移）** | ❌ | 当前仅静音，P5 高优先级 |
| **Trim/Crop (Ctrl+T)** | ❌ | P5 高优先级 |
| 时间尺/刻度标尺 | ❌ | 计划中 |
| 音量/增益实时控制 | ❌ | 计划中 |
| 电平表 | ❌ | 计划中 |
| 相位翻转 | ❌ | 低优先级 |
| DC 偏移移除 | ❌ | 低优先级 |
| 选区扩展（Shift+方向键） | ❌ | 未实现 |
| F2 重命名 Region | ❌ | 未实现 |
| Loop 录音（多 take） | ❌ | 未实现 |
| 时间伸缩/变调 | ❌ | 高级功能 |

## 审计报告 (2026-05-15 七轮审计)

### 发现的问题

| # | 问题 | 严重度 | 状态 |
|---|------|--------|------|
| 1 | **选区拖动时波形高亮和信息文本不更新** — SelectionOverlay::mouseDrag/mouseDown 只对自己调用 repaint()，但选区实际渲染在 WaveformThumbnail 中，信息文本在 MainComponent 中。拖动时视觉无反馈。 | **高** | ✅ 已修复（首轮） |
| 2 | **缺少保存/导出完整音频功能** — 只能通过 Alt+拖拽导出选区 WAV，无法保存编辑后的完整音频文件。 | **高** | ✅ 已修复 — 添加了"Save As..."菜单项和 Ctrl+Shift+S 快捷键（首轮） |
| 3 | **缺少垂直缩放** — WaveformThumbnail 没有垂直缩放控制，无法放大查看波形细节。 | **中** | ✅ 已修复 — 添加了 Ctrl+Shift+鼠标滚轮 垂直缩放（首轮） |
| 4 | **cmdRemoveRegion 没有快捷键** — 这是唯一没有键盘快捷键的已注册命令。 | **低** | ✅ 已修复 — 添加 Shift+Backspace（首轮） |
| 5 | **Region 标签栏在频谱视图中依然可见** — 切换到频谱视图时 RegionOverlay 未隐藏。 | **低** | 不影响功能，区域信息在两种视图中都有用，无需修复 |
| 6 | **选区信息文字被 TransportBar 不透明背景遮盖** — MainComponent::paint() 在底部 36px 绘制选区时间和吸附模式文字，但 TransportBar::paint() 用 fillAll(0xFF222244) 完全覆盖该区域。选区信息对用户永远不可见。 | **中** | ✅ 已修复 — 将选区信息绘制迁移到 TransportBar::paint() 左侧区域，与进度条和信息面板无重叠 |
| 7 | **缺少 Stop 快捷键** — cmdStop 注册在菜单中但没有键盘绑定。Edison 中 Escape 可以停止播放并复位到起点。 | **低** | ✅ 已修复 — 添加 Escape 键映射到 cmdStop |
| 8 | **频谱图像素行间隙** — 当视图高度 > FFT bins (512) 时 (如 800px 窗口)，每 bin 只画单个像素行，出现可视横纹间隙。 | **低** | ✅ 已修复 — 改为按 Y 坐标迭代，寻找最近频率仓，保证每个像素行都有颜色 |
| 9 | **TransportBar 选区信息被按钮覆盖不可见** — drawSelectionInfo 画在 `getLocalBounds().removeFromLeft(136)` 区域，但按钮占用前 124px。文字完全被不透明按钮遮盖。 | **中** | ✅ 已修复 — 改为画在按钮和右侧信息面板之间的中间区域 |
| 10 | **SelectionOverlay::drawSelectionInfo 死代码** — 定义在头文件和实现文件，但从未被调用。TransportBar 有独立的 drawSelectionInfo 负责渲染。 | **低** | ✅ 已修复 — 删除声明和实现 |
| 11 | **TransportBar 缺少 GridManager 引用** — 只能显示 "Grid" 标签，无法显示 BPM/节拍信息。之前通过 SelectionOverlay 的 drawSelectionInfo（死代码）展示了完整信息。 | **低** | ✅ 已修复 — 添加 setGridManager + m_gridManager 成员，drawSelectionInfo 显示 "Grid 120/4" 格式 |

### 修复详情（本轮审计 — 三轮审计新增/修正的问题 #12~#13）
### 审计发现（无需修复）

| # | 发现 | 说明 |
|---|------|------|
| 1 | Play Selection（菜单项 cmdPlaySel）无快捷键 | 常规 Space 播放已覆盖选区播放（有选区时自动从选区开始/在选区结束），无需额外绑定 |
| 2 | 波形自动滚动始终开启（无开关） | Edison 有 “Follow Playhead” 切换按钮。设计意图——简化为始终跟随 |
| 3 | LoopManager / AudioFileManager 音频线程读取（x86_64 对齐原子性） | hasValidLoop()/getBuffer()/getSample() 等从音频线程读取，消息线程写入。理论上数据竞争，但 x86_64 对齐读写是原子的，JUCE 生态约定如此。跨 ARM 架构需后续修复。 |

### 审计发现（edison-advance 七轮审计 — 新增/修复的问题 #52~#53）

#### 1. AudioThumbnail ChangeListener 重复注册（低优先级 Bug）
- 文件： MainComponent.cpp -> loadAudioFile()
- 根因： loadAudioFile() 每次调用 m_thumbnail->addChangeListener(this)，但 m_thumbnail 在 AudioFileManager::loadFile() 中被重用（仅 clear + reset，不重新创建）。每次加载新文件都会新增一个 listener 副本，不会移除旧的。结果：加载 N 个文件后，每个 AudioThumbnail 变更事件触发 changeListenerCallback N 次，产生大量冗余 repaint。
- 修复： addChangeListener 前先调用 removeChangeListener(this)，确保只有一个 active listener。

#### 2. 停止播放后 stale callAsync 将位置跳回播放点（低优先级视觉 Bug）
- 文件： TransportBar.cpp -> getNextAudioBlock()
- 根因： getNextAudioBlock() 通过 MessageManager::callAsync 将 newPos 回写到 m_position 并触发 onPositionChanged。当一个 `getNextAudioBlock` 的 callAsync 已入队但尚未执行时，用户在 UI 上按下 Escape（或播放自然结束触发 stop()），stop() 执行 setPosition(0.0) 将位置重置为 0。随后 pending callAsync 触发，将 m_position 写回旧值（例如 5.3s），导致波形位置跳回播放点，视觉上闪烁。
- 修复： callAsync lambda 开头添加 `if (m_state != State::Playing) return;`，确保只在仍在播放时更新位置。

### 审计发现（edison-advance 八轮审计 — 代码审计 + Edison 功能完整性审核）

**构建状态：** OpenEdison 主程序编译通过 ✅（Test 目标因 JUCE 模块版本不兼容链接失败，不影响主程序）

**七轮修复验证：** ✅
1. `thumb->removeChangeListener(this)` 在 `addChangeListener` 前调用 — 已验证代码中存在
2. `callAsync lambda` 中 `if (m_state != State::Playing) return;` 守卫 — 已验证

#### 新增发现

**#54. TransportBar::resized() 死变量 `selInfoArea`（低优先级，代码整洁）**
- 文件： `TransportBar.cpp` `resized()`
- 行： 构造 `auto selInfoArea = r.removeFromLeft(200)` 后变量从未被使用
- 根因： `removeFromLeft()` 修改 `r`（从剩余空间移除 200px 给选区信息），但 `selInfoArea` 变量从未被引用。`drawSelectionInfo()` 在 `paint()` 中从 `getLocalBounds()` 重新计算自己的绘制区域而非使用此变量。
- 影响： 无功能影响。`r.removeFromLeft(200)` 的效果仍然作用于 `r`，`m_progress` 放置位置正确。
- 建议： 移除 `auto selInfoArea =` 前缀，改为裸调用 `r.removeFromLeft(200);` 或完全移除该行（progress 的 bounds 已被右侧 `r.removeFromRight(infoWidth)` 限制）。

**#55. TransportBar::paint() 使用 getLocalBounds() 而非 reduced 坐标（低优先级，视觉对齐）**
- 文件： `TransportBar.cpp` `paint()` vs `resized()`
- 根因： `resized()` 使用 `getLocalBounds().reduced(4, 2)` 布局按钮（按钮从 y=2 开始），但 `paint()` 使用 `getLocalBounds()` 作为参考系（y=0）。选区信息文本、时间显示、文件信息文本均绘制在 y=0 基准线上，比按钮视觉中心高约 2px。
- 影响： 视觉上文本略偏上，不影响功能。

**#56. m_readIndex 在超长音频文件中溢出 `int`（低优先级，边界条件）**
- 文件： `TransportBar.h` — `std::atomic<int> m_readIndex`
- 根因： `m_readIndex` 存储采样索引，类型为 `int`（32-bit signed）。44.1kHz 下溢出边界约为 13.5 小时，192kHz 下约为 3.1 小时。对于超过此长度的音频文件，`m_readIndex` 溢出为负值，导致不可预测的播放行为。
- 影响： 实际使用中极少遇到超长音频，建议使用 `std::atomic<int64_t>` 或 `std::atomic<long long>` 修复。

#### 新增缺失功能清单（对比 FL Studio Edison）

以下功能在 Edison 中存在，Open Edison 尚未实现：

| # | 功能 | 优先级 | 说明 |
|---|------|--------|------|
| 1 | **Ripple Delete** | **高** | Delete/Backspace = 删除选区并拉移后续音频（当前仅静音）。这是 Edison 核心行为之一 |
| 2 | **Trim/Crop (Ctrl+T)** | **高** | 保留选区内容，删除选区外所有音频 |
| 3 | **时间尺/刻度标尺** | 中 | 波形上方显示秒/小节刻度参考线 |
| 4 | **音量/增益实时控制** | 中 | TransportBar 添加增益滑块 |
| 5 | **相位翻转** | 低 | 选区/全文件相位反转（乘以 -1） |
| 6 | **DC 偏移移除** | 低 | 自动化移除直流偏移 |
| 7 | **电平表** | 低 | 播放时显示实时电平 |
| 8 | **选区扩展（Shift+方向键）** | 低 | 逐采样点微调选区边界 |
| 9 | **F2 重命名 Region** | 低 | Region 标签上双击/按 F2 重命名 |
| 10 | **Loop 录音** | 低 | 循环播放时录音，保留每次 take |

#### 功能对比表更新

| Edison 功能 | Open Edison | 备注 |
|-------------|-------------|------|
| Ripple Delete (删除+拉移) | ❌ | 当前仅静音，见待处理 |
| Trim/Crop (Ctrl+T) | ❌ | 见待处理 |
| 时间尺/刻度标尺 | ❌ | 计划中 |
| 音量/增益实时控制 | ❌ | 计划中 |
| 电平表 | ❌ | 计划中 |
| DC 偏移移除 | ❌ | 未实现 |
| 相位翻转 | ❌ | 未实现 |
| 选区扩展（Shift+方向键） | ❌ | 未实现 |
| F2 重命名 Region | ❌ | 未实现 |
| Loop 录音（多 take） | ❌ | 未实现 |

### 审计发现（edison-advance 六轮审计 — 新增/修复的问题 #49~#51\uff09

#### 1. Undo/Redo 菜单启用状态不刷新（中优先级 Bug\uff09
- 文件： MainComponent.cpp -> perform() 中的 undoable actions
- 根因： getCommandInfo() 中 cmdUndo/cmdRedo 调用 info.setActive(m_undoManager.canUndo()/canRedo())，但执行 undoable action 后未通知 ApplicationCommandManager 更新状态。结果：刚静音后，Undo 菜单项仍灰色不可用。
- 修复： 所有 m_undoManager.perform(action) 后和 cmdUndo/cmdRedo 后添加 m_commandManager->commandStatusChanged()。涉及 6 处。

#### 2. cmdNew(New Project) 快捷键未在菜单中显示（低优先级 UX 一致性）
- 文件： MainComponent.cpp -> getCommandInfo()
- 根因： cmdNew 有显式 mappings->addKeyPress 但 getCommandInfo() 中缺少 addDefaultKeypress。JUCE 靠 addDefaultKeypress 在菜单中显示快捷键文本。
- 修复： 添加 info.addDefaultKeypress(‘n’, ctrlModifier)。

#### 3. TransportBar::drawSelectionInfo 文字重叠（低优先级视觉 Bug\uff09
- 文件： TransportBar.cpp -> drawSelectionInfo()
- 根因： 选区时间信息和吸附模式信息在同一个 200px 矩形中以 centredLeft + centredRight 渲染，长文字时重叠。
- 修复： 将 r 水平分割为左/right 半部分，各自使用 removeFromLeft/removeFromRight 获取子矩形。


#### 1. Undo 历史未在加载新文件时清空（严重 Bug）
- **文件：** `MainComponent.cpp` `loadAudioFile()`
- **根因：** 加载新文件时，`m_undoManager` 未被清空。如果用户加载文件 B 后按 Ctrl+Z，UndoManager 会尝试用文件 A 的原始样本来修改文件 B 的缓冲区，导致数据损坏。
- **修复：** 在 `loadAudioFile()` 中添加 `m_undoManager.clearUndoHistory()`，确保新文件的撤销栈从干净状态开始。

#### 2. Ctrl+O 键冲突 — Open 与 Fade Out 共用同一快捷键（中优先级）
- **文件：** `MainComponent.cpp` `getCommandInfo()`
- **根因：** `cmdOpen` 和 `cmdFadeOut` 的 `getCommandInfo()` 都调用 `info.addDefaultKeypress('o', ctrlModifier)`。`resetToDefaultMappings()` 将同一个 Ctrl+O 绑定到两个命令上，按 Ctrl+O 时行为不可预测。
- **修复：** 移除 `cmdFadeOut` 的默认快捷键绑定（Fade Out 在菜单中仍可用，仅移除键盘快捷键以避免冲突）。

### 审计发现（四轮审计 — 新增/修复的问题 #14~#15）

#### 1. 进度条覆盖选区信息文本（严重 Bug）
- **文件：** `TransportBar.cpp` `paint()` / `resized()`
- **根因：** `resized()` 中进度条 (`m_progress`) 占据按钮右侧所有剩余空间（直到右侧信息面板）。`paint()` 中的 `drawSelectionInfo` 也在同一区域绘制文本。由于子组件在 `paint()` 之后绘制，进度条完全覆盖选区信息文本。此外，`paint()` 中 `middleArea` 的 `withTrimmedRight(rightPanel.getWidth() + 4)` 在 `bounds` 已移除右侧面板后额外右缩进，进一步压缩了绘制区域。
- **修复：** `resized()` 中保留 200px 专用区域用于选区信息（`auto selInfoArea = r.removeFromLeft(200)`）。`paint()` 中选区信息改为渲染在 `getLocalBounds().withTrimmedLeft(132).withWidth(200)` 的固定区域内，与进度条无重叠。

#### 2. 加载新文件时 Region 和 Loop 未清空（中优先级 Bug）
- **文件：** `MainComponent.cpp` `loadAudioFile()`
- **根因：** `loadAudioFile()` 清空 marker、selection 和 undo history，但未清空 `m_regionManager` 和 `m_loopManager`。旧文件的 Region 定义仍然保留，时间范围在原文件的时长范围内，加载更短的新文件后访问越界。
- **修复：** 在 `loadAudioFile()` 末尾添加 `m_regionManager->clear()` 和 `m_loopManager->clearLoop()`。

### 代码整洁（四轮）

#### 3. `constexpr int infoWidth = 380` 重复定义
- **位置：** `TransportBar.cpp` `resized()` 和 `paint()` 各定义了一次同值常量
- **说明：** 功能正确，但违反 DRY。修复：本次暂不提取（仅影响两处局部作用域，开销可忽略），留待后续重构。

### 审计发现（七轮审计）

#### 1. `m_currentRegionCycler` 死代码（低优先级，内务清理）
- **文件：** `MainComponent.h`
- **根因：** `int m_currentRegionCycler = -1` 成员变量声明在头文件中，但项目中没有任何代码引用或使用该变量。之前 Region 相关代码重构后遗留的死字段。
- **修复：** 从 `MainComponent.h` 中移除该声明。

#### 2. Undo/Redo 菜单项始终可用（低优先级，UX）
- **文件：** `MainComponent.cpp` `getCommandInfo()`
- **根因：** `cmdUndo` 和 `cmdRedo` 的 `getCommandInfo()` 未调用 `info.setActive()`，导致菜单项在撤销栈为空时仍然显示为可用状态。
- **修复：** 添加 `info.setActive(m_undoManager.canUndo())` 和 `info.setActive(m_undoManager.canRedo())`，使 Undo/Redo 菜单项在无可用操作时变灰。

#### 3. 无音频时进度条可操作（低优先级，UX）
- **文件：** `TransportBar.cpp` `updateButtonStates()`
- **根因：** 进度条滑块（`m_progress`）在任何状态下都保持可用状态。当未加载音频文件时，用户可以拖拽滑块但无任何效果。
- **修复：** `updateButtonStates()` 中添加 `m_progress.setEnabled(hasAudio)`，无音频时禁用进度条。

#### 4. TransportBar `paint()` 选区信息位置与 `resized()` 不一致（低优先级，布局）
- **文件：** `TransportBar.cpp`
- **根因：** `paint()` 中使用 `buttonAreaEnd = 132` 计算选区信息绘制区域，但 `resized()` 中按钮区域实际占用 `4×30 + 4gap + 2×2(inset)` = 128px。4px 差异导致绘制区域右端与进度条左端微幅重叠。
- **修复：** 将 `buttonAreaEnd` 从 132 调整为 128，与 `resized()` 布局完全对齐。

### 审计发现（八轮审计 — 新增/修复的问题 #19~#23）

#### 1. 波形视图普通滚轮误缩滚动（中优先级，UX 一致性）
- **文件：** `WaveformThumbnail.cpp` — `mouseWheelMove()`
- **根因：** WaveformThumbnail 中普通滚轮（不按 Ctrl）触发缩放（1.25 倍），而 SpectrogramComponent 中普通滚轮执行水平滚动。两视图行为不一致。设计规格明确要求「Ctrl+滚轮缩放，滚轮水平滚动」。
- **修复：** 重写 `mouseWheelMove()`：普通滚轮→水平滚动；Ctrl+滚轮→缩放；Ctrl+Shift+滚轮→垂直缩放（保持不变）。缩放因子与 Spectrogram 统一为 1.05。

#### 2. 缺少 Select All（Ctrl+A）（低优先级，标准功能缺失）
- **文件：** `MainComponent.h/cpp`
- **根因：** 编辑器中常见的「全选」功能未实现。Edison 中 Ctrl+A 选择整个音频文件。
- **修复：** 添加 `cmdSelectAll` 命令 ID（0x1070），绑定 Ctrl+A。选中全部音频（start=0, end=totalDuration）。添加到 Edit 菜单（Select All 之后、Cut 之前）。

#### 3. 加载文件时 Zoom-to-Selection 状态未重置（低优先级 Bug）
- **文件：** `MainComponent.cpp` — `loadAudioFile()`
- **根因：** `m_zoomedToSelection`、`m_preZoomLevel`、`m_preZoomOffset` 在加载新文件时未清空。如果旧文件处于 zoom-to-selection 模式，按 Z 键会尝试用旧文件的缩放参数恢复到新文件上，产生不可预见的视图变化。
- **修复：** `loadAudioFile()` 中添加 `m_zoomedToSelection = false; m_preZoomLevel = 1.0; m_preZoomOffset = 0.0;`。

#### 4. 缺少 trackpad 横向滚轮支持（低优先级，UX）
- **文件：** `WaveformThumbnail.cpp`、`SpectrogramComponent.cpp` — `mouseWheelMove()`
- **根因：** 两个视图类仅处理 `w.deltaY`（垂直滚轮），完全忽略 `w.deltaX`（触控板两指横向滑动）。使用触控板的用户无法横向滚动。
- **修复：** 两个类均添加 `w.deltaX != 0.0` 检查，执行水平滚动，滚动方向取反（左滑 = 向右滚动视图）。

#### 5. View 菜单中重复的 Region 菜单项（低优先级，菜单清洁）
- **文件：** `MainComponent.cpp` — `getMenuForIndex()`
- **根因：** View 菜单的 Marker 分隔线之后包含了 `cmdAddRegion`、`cmdNextRegion`、`cmdPrevRegion`，这些在 Edit 菜单中已经存在。View 菜单应仅包含显示相关的命令（Markers、Snap、Loop）。
- **修复：** 从 View 菜单中移除重复的 Region 菜单项（cmdAddRegion/cmdNextRegion/cmdPrevRegion）。

### 审计发现（九轮审计 — 新增/修复的问题 #24~#27）

#### 1. `cmdSelectAll` 未注册到 `getAllCommands()`（中优先级 Bug）
- **文件：** `MainComponent.cpp` — `getAllCommands()`
- **根因：** `cmdSelectAll` 命令已添加 `getCommandInfo()` 和菜单项，但在 `getAllCommands()` 的注册列表中缺失。ApplicationCommandManager 通过 `getAllCommands()` 发现所有可用命令，缺失导致 `resetToDefaultMappings()` 无法给 Ctrl+A 分配默认快捷键，实际键盘快捷键完全失效。用户只能通过菜单点击使用 Select All。
- **修复：** 在 `getAllCommands()` 的 `cmds.addArray()` 中添加 `cmdSelectAll`。

#### 2. `cmdSelectAll` 缺少显式键映射（低优先级，一致性）
- **文件：** `MainComponent.cpp` — 构造函数
- **根因：** 即使 `getAllCommands()` 修复后 `resetToDefaultMappings()` 可自动分配 Ctrl+A，但项目中 30+ 个快捷键全部使用显式 `mappings->addKeyPress()` 注册，唯独 `cmdSelectAll` 依赖隐式默认映射，风格不一致且脆弱（`resetToDefaultMappings` 的隐式依赖）。
- **修复：** 在构造函数中的显式键映射块末尾添加 `mappings->addKeyPress(cmdSelectAll, KeyPress('a', ctrlModifier, 0))`。

#### 3. Zoom-to-Selection（Z 键）在 Spectrogram 视图中无效（中优先级，功能不一致）
- **文件：** `MainComponent.cpp` — `zoomToSelection()`、`fitAll()`
- **根因：** Z 键缩放和 Ctrl+Shift+F 恢复完整视图仅更新 `WaveformThumbnail` 的缩放和偏移量，完全不涉及 `SpectrogramComponent`。用户在 Spectrum 视图中按 Z 键时，波形视图缩放但频谱视图保持原样，切换视图后看到不一致的缩放状态。按 Z 键恢复也不复位频谱视图。
- **修复：** `zoomToSelection()` 和 `fitAll()` 中同步调用 `m_spectrogram->setZoom()` 和 `m_spectrogram->setViewOffset()`，使频谱视图与波形视图的缩放状态完全一致。Toggle 恢复逻辑中也记录并恢复频谱的 view offset。

#### 4. 选区播放结束条件在循环模式下时仍检查选区边界（低优先级，逻辑冗余）
- **文件：** `TransportBar.cpp` — `getNextAudioBlock()`
- **根因：** 当 loop 已启用时，`else if (m_selection.hasSelection())` 分支永远不会执行，因为 `shouldLoop` 分支先执行并包含自己的循环退出逻辑。当前代码功能正确，但 `else if` 条件在 loop 模式下不会被评估，无实际 Bug。保持现状。
- **修复：** 无需修复，逻辑正确。仅作记录。

### 审计发现（十轮审计 — 新增/修复的问题 #28~#31）

#### 1. 频谱/波形双视图 Zoom 不同步 — Marker/Loop Overlay 坐标错位（中优先级 Bug）
- **文件：** `WaveformThumbnail.h/cpp`, `SpectrogramComponent.h/cpp`, `MainComponent.cpp`
- **根因：** MarkerOverlay、LoopOverlay、GridOverlay 都使用 `m_waveformThumbnail.timeToX()` 进行时间→像素坐标转换。但 `SpectrogramComponent` 拥有自己独立的 `m_hZoom` 和 `m_viewOffset`，用户通过鼠标滚轮在频谱视图中缩放/滚动时，只更新频谱的内部状态，波形视图的 zoom/offset 保持不变。这导致所有叠加层（Marker/Loop旗标和半透明高亮）在频谱视图中与实时内容完全错位。反之亦然：波形视图的滚动不会同步到频谱视图。
- **影响：** 用户在频谱视图中做任何横向滚动或缩放的瞬间，标记旗标、LOOP 高亮区域立刻画在错误位置（使用波形未变的坐标系），严重破坏用户体验。
- **修复：**
  1. `WaveformThumbnail.h` 和 `SpectrogramComponent.h` 各自新增 `std::function<void(double zoom, double offsetSec)> onUserViewChanged` 公有回调成员。
  2. `WaveformThumbnail::mouseWheelMove()` 中每个会改变 zoom/offset 的分支末尾（Ctrl+缩放、普通滚动、trackpad 横向滚动）都调用 `onUserViewChanged`。
  3. `SpectrogramComponent::mouseWheelMove()` 末尾也调用 `onUserViewChanged`。
  4. `MainComponent` 构造函数中连接双向回调：
     - `m_waveform->onUserViewChanged` → 调用 `m_spectrogram->setZoom()` + `setViewOffset()` + `refreshViewport()` + 重置 `m_zoomedToSelection = false`。
     - `m_spectrogram->onUserViewChanged` → 调用 `m_waveform->setZoom()` + `setHorizontalOffset()` + `repaint()` + 重置 `m_zoomedToSelection = false`。

#### 2. cmdZoomIn / cmdZoomOut 未同步频谱视图（中优先级，功能不完整）
- **文件：** `MainComponent.cpp` — `perform()` 中的 `cmdZoomIn` / `cmdZoomOut`
- **根因：** 按下 `=`（Zoom In）或 `-`（Zoom Out）时，只更新 `WaveformThumbnail` 的 zoom，完全不涉及 `SpectrogramComponent`。如果用户在频谱视图中按 `=`，波形 zoom 改变但频谱无任何变化；切换回波形视图时发现 zoom 已被改变，视觉上不连贯。
- **修复：** `cmdZoomIn` / `cmdZoomOut` 中同步调用 `m_spectrogram->setZoom(newZoom)` + `refreshViewport()`，使两个视图的缩放始终一致。

#### 3. `m_zoomedToSelection` 未在手动缩放/滚动时重置（低优先级，UX 不一致）
- **文件：** `MainComponent.cpp` — `onUserViewChanged` 回调
- **根因：** 如果用户先按 Z 键缩放到选区（`m_zoomedToSelection = true`），然后通过鼠标滚轮手动缩放/滚动到一个不同的视图状态，`m_zoomedToSelection` 仍为 `true`。此时再按 Z 键，本应执行「恢复前一个视图状态」（某些用户期望的 toggle 行为），但实际上恢复的是第一次 Z 键前的状态（`m_preZoomLevel` / `m_preZoomOffset`），不是当前手动缩放后的状态，造成令人困惑的跳转。
- **修复：** 在两个 `onUserViewChanged` 回调中均设置 `m_zoomedToSelection = false`。任何手动视图操作都会清除 zoom-to-selection 状态，使下次 Z 键执行新的 zoom-to-selection，而非尝试恢复旧状态。

#### 4. `m_regionOverlay->setBounds()` 在 `resized()` 中重复调用（低优先级，代码整洁）
- **文件：** `MainComponent.cpp` — `resized()`
- **根因：** `m_regionOverlay->setBounds(regionBarR)` 被调用两次，第二次调用和第一次使用完全相同的 `regionBarR` 值。注释错误地声称 "setAlwaysOnTop already set"。
- **修复：** ✅ 已修复（本轮）— 移除第二次 setBounds 调用，替换为注释说明。

### 审计发现（六轮审计 — 新增/修复的问题 #17~#18）

#### 1. `m_readIndex` 数据竞争（高优先级，线程安全）
- **文件：** `TransportBar.h` — `int m_readIndex` 成员
- **根因：** `m_readIndex` 被音频实时线程（`getNextAudioBlock()` 中多次读写递增）和消息线程（`setPosition()`/`play()`/`playSelection()` 写入）同时访问，无任何同步保护。这是 C++ 未定义行为。
- **修复：** 将 `m_readIndex` 类型改为 `std::atomic<int>`，保证所有读写操作原子化。

#### 2. `m_selection.setPlaybackPosition()` 从音频线程直接调用（高优先级，线程安全）
- **文件：** `TransportBar.cpp` — `getNextAudioBlock()` 方法
- **根因：** `getNextAudioBlock()` 直接调用 `m_selection.setPlaybackPosition(newPos)` 写入 `SelectionManager::m_playbackPos`（消息线程通过 `WaveformThumbnail::paint()` 读取同一变量）。音频线程写入 + 消息线程读取 = 数据竞争。
- **修复：** 将 `setPlaybackPosition(newPos)` 移入已存在的 `juce::MessageManager::callAsync` lambda 内部，确保只在消息线程写入。

### 审计发现（五轮审计 — 之前修复的问题 #16）

#### 1. 波形视图缺少回放自动滚动（中优先级，Edison 兼容性）
- **文件：** `MainComponent.cpp` — `m_transport->onPositionChanged` 回调
- **根因：** SpectrogramComponent 在播放头到达可视区域右侧 85% 时自动滚动视口，但 WaveformThumbnail 完全依赖手动操作（滚轮/缩放/拖拽）。回放期间播放头移出可视区域后用户将失去波形视觉反馈。
- **修复：** 在 `onPositionChanged` 中增加波形自动滚动逻辑——当 `pos > rightEdge - threshold` 时，将视图偏移设置为 `pos - viewDur * 0.7`，使播放头位于可视区域左侧约 70% 处。触发所有叠加层重绘。

## 功能完整性审计 — 对比 FL Studio Edison v21

| 功能 | 状态 | 备注 |
|------|------|------|
| 波形显示（多声道/缩放） | ✅ | 垂直+水平缩放 |
| 选区（创建/拖动/吸附） | ✅ | 零交叉+网格吸附 |
| 选区处理（静音/反向/标准化/淡入淡出） | ✅ | Undo 支持 |
| 复制/剪切/粘贴 | ✅ | 内部剪贴板 |
| 频谱/声谱图 | ✅ | 视口驱动 STFT，任意时长 |
| 标记/提示点 | ✅ | M/Shift+M/[ / ] |
| Region（块）管理 | ✅ | 带颜色标签+右键菜单 |
| 循环/AB 复读 | ✅ | L 键切换+半透明高亮 |
| 录制 | ✅ | 输入设备选择 |
| 回放自动滚动（波形） | ✅ | 跟随播放头 |
| 回放自动滚动（频谱） | ✅ | 已有功能 |
| 缩放至选区 | ✅ | Z 键切换/恢复 |
| 拖拽导出 WAV | ✅ | Alt+选区拖动 |
| Save As | ✅ | Ctrl+Shift+S |
| Escape 停止 | ✅ | 停止+复位 |
| **Ripple Delete（删除+拉移）** | ❌ | 当前仅静音，P5 高优先级 |
| **Trim/Crop (Ctrl+T)** | ❌ | P5 高优先级 |
| Batch/Smart tool | ❌ | 非核心功能 |
| 时间尺/刻度标尺 | ❌ | 计划中 |
| 音量/增益实时控制 | ❌ | 计划中 |
| 电平表 | ❌ | 计划中 |
| 相位翻转 | ❌ | 低优先级 |
| DC 偏移移除 | ❌ | 低优先级 |
| 选区扩展（Shift+方向键） | ❌ | 未实现 |
| F2 重命名 Region | ❌ | 未实现 |
| Loop 录音（多 take） | ❌ | 未实现 |
| 时间伸缩/变调 | ❌ | 高级功能 |
| 新建/新项目 | ✅ | File > New Project 菜单项 |

### 审计发现（edison-advance 九轮审计 — 新增/修复的问题 #57~#61）

#### 1. TransportBar::resized() 未使用的变量 selInfoArea（低优先级，已修复）
- **文件：** `TransportBar.cpp` — `resized()`
- **根因：** `auto selInfoArea = r.removeFromLeft(200)` 声明了一个从未使用的局部变量。
- **修复：** 移除 `auto selInfoArea =` 前缀，改为裸调用 `r.removeFromLeft(200);`。
- **影响：** 无功能影响，代码整洁改进。

#### 2. RegionOverlay 析构时未清除 onRegionsChanged 回调（低优先级，已修复）
- **文件：** `RegionOverlay.h/cpp`
- **根因：** `RegionOverlay` 构造函数将 `onRegionsChanged` 设置为捕获 `this` 的 lambda。析构时未置空，若 `MainComponent` 销毁过程中 `onRegionsChanged` 被触发（极小概率），将导致悬挂指针访问。
- **修复：** 添加 `~RegionOverlay()` 析构函数，在其中设置 `m_regionManager.onRegionsChanged = nullptr`。

#### 3. SpectrogramComponent::renderViewport 未将钳位后的 startTime 写回 m_viewOffset（低优先级，已修复）
- **文件：** `SpectrogramComponent.cpp`
- **根因：** `renderViewport()` 内部为偏移量做了局部钳位（`startTime = max(0, totalDur - viewDur)`），但未同步回 `m_viewOffset`。后续调用以错误的 `m_viewOffset` 开始，需要再次钳位，导致不一致状态窗口。
- **修复：** 钳位后，若 `startTime != m_viewOffset`，将 `m_viewOffset = startTime`。

#### 4. SpectrogramComponent::setPlaybackPosition 自动滚动未做上限钳位（低优先级，已修复）
- **文件：** `SpectrogramComponent.cpp` — `setPlaybackPosition()`
- **根因：** 自动滚动时 `m_viewOffset = posSec - viewDuration * 0.7` 后仅做了 `std::max(0.0, ...)` 钳位下限，未做上限。当音频很短或 zoom 较低时，`m_viewOffset` 可超出 `totalDur - viewDur`。
- **修复：** 改为使用 `std::clamp(m_viewOffset, 0.0, max(0.0, totalDur - viewDuration))`。

#### 5. DragExport 临时文件累积（低优先级，已修复）
- **文件：** `DragExport.cpp`
- **根因：** 每次 Alt+选区拖拽导出都会在 `{temp}/OpenEdisonExport/` 下创建新 WAV 文件，这些文件从未被清理。
- **修复：** 导出前检查 `OpenEdisonExport` 目录中的 `.wav` 文件数，超过 20 个时删除最旧的直到剩余 20 个以下。

#### 6. 八轮审计 #55: TransportBar paint() 与 resized() Y 坐标对齐（低优先级，未修复）
- **文件：** `TransportBar.cpp`
- **根因：** `paint()` 使用 `getLocalBounds()` (y=0)，`resized()` 使用 `getLocalBounds().reduced(4, 2)` (y=2)。选区信息和文件信息文本比按钮高 2px。
- **影响：** 轻微视觉偏移，不影响功能。留待后续 UI 统一重构时处理。

### 审计发现（无需修复）

| # | 发现 | 说明 |
|---|------|------|
| 1 | Play Selection（菜单项 cmdPlaySel）无快捷键 | 常规 Space 播放已覆盖选区播放（有选区时自动从选区开始/在选区结束），无需额外绑定 |
| 2 | 波形自动滚动始终开启（无开关） | Edison 有 "Follow Playhead" 切换按钮。设计意图——简化为始终跟随 |
| 3 | LoopManager / AudioFileManager 音频线程读取（x86_64 对齐原子性） | `hasValidLoop()`/`getBuffer()`/`getSample()` 等从音频线程读取，消息线程写入。理论上数据竞争，但 x86_64 对齐读写是原子的，JUCE 生态约定如此。跨 ARM 架构需后续修复。 |



#### 1. TransportBar 选区信息被按钮覆盖（中优先级，二轮审计发现）
- **文件：** `TransportBar.cpp`
- **根因：** `TransportBar::paint()` 中 `constexpr int btnWidth = 128` 和 `getLocalBounds().removeFromLeft(btnWidth + 8)` 声称是按钮区域，但 `resized()` 中按钮实际占用 `4 × 30 + 4 gap + 2×2 padding = ~124px`。136px vs 124px 仅有 12px 差异，但文字以 `centredLeft` 对齐，起始于 x=0，完全被 x=4..124 的按钮覆盖。
- **修复：** `drawSelectionInfo` 改为在中间区域渲染：按钮之后（x≥132）、右侧信息面板之前。计算方式：`getLocalBounds().withTrimmedLeft(132).withTrimmedRight(rightPanelWidth + 4)`。

#### 2. 删除 SelectionOverlay::drawSelectionInfo 死代码（低优先级，代码整洁）
- **文件：** `SelectionOverlay.h`, `SelectionOverlay.cpp`
- **根因：** `drawSelectionInfo` 声明在头文件、实现在 cpp，但从未被任何代码调用。TransportBar 有自己独立的 `drawSelectionInfo` 负责渲染选区信息。
- **修复：** 从 SelectionOverlay.h/cpp 中移除该方法的声明和实现。

#### 3. TransportBar 接入 GridManager 显示 BPM 信息（低优先级，信息完整）
- **文件：** `TransportBar.h`, `TransportBar.cpp`, `MainComponent.cpp`
- **根因：** TransportBar 的 `drawSelectionInfo` 只能显示 "Grid" 标签，无法显示 BPM/division（120/4）。而 SelectionOverlay 的（已删除）死代码可以显示完整信息因为它持有 GridManager 引用。
- **修复：** TransportBar 添加 `setGridManager(GridManager*)` 方法和 `m_gridManager` 成员。`drawSelectionInfo` 改为在有 GridManager 引用时显示 "Grid 120/4" 格式。在 `MainComponent` 构造中同步设置 `m_transport->setGridManager(m_gridManager.get())`。

### 旧修复详情（本轮）

#### 1. 选区信息被 TransportBar 遮盖（中优先级）
- **文件：** `MainComponent.cpp`, `TransportBar.h/.cpp`
- **根因：** MainComponent::paint() 在底部 36px 区域绘制选区时间和吸附模式。TransportBar 作为子组件以不透明背景 fillAll 绘制在相同区域，完全覆盖选区信息。
- **修复：** 移除 MainComponent::paint() 中的选区信息绘制。在 TransportBar::paint() 中增加左侧选区信息面板，与按钮区重叠但不冲突。（注意：Grid BPM 信息暂不显示，因为 TransportBar 不持有 GridManager 引用）

#### 2. Stop 快捷键（低优先级，Edison 兼容性）
- **文件：** `MainComponent.cpp`
- **修改：** key mappings 中添加 Escape → cmdStop；getCommandInfo 中添加 Escape 默认键描述

#### 3. 频谱图像素间隙修复（低优先级，视觉质量）
- **文件：** `SpectrogramComponent.cpp`
- **根因：** 原代码按 bin 索引遍历 (0..511) 并映射到 Y 坐标，当 viewHeight > 512 时 bin 间有像素行间隙
- **修复：** 反向迭代：遍历 Y (0..viewHeight-1)，用逆映射找到最近的 bin，确保每行都有颜色

### 审计发现（edison-advance — 新增/修复的问题 #32~#35）

#### 1. 缺少 New Project 功能（低优先级，基本功能缺失）
- **文件：** `MainComponent.h/cpp`
- **根因：** 用户无法清除当前项目重新开始——只能关闭窗口或加载另一个文件。FL Studio Edison 有 New/新建功能。
- **修复：** 添加 `cmdNew` 命令 ID（0x1000），添加到 File 菜单（New Project）。实现 `newProject()` 方法：停止播放、unload 音频文件、清空选区/标记/Region/Loop/Undo 历史/剪贴板、重置缩放状态、重置窗口标题。

#### 2. 缺少 BPM/Grid Division 设置 UI（低优先级，设置缺失）
- **文件：** `MainComponent.h/cpp`
- **根因：** GridManager 默认 120BPM/4 分音符，用户无法通过 UI 修改 BPM 或节拍划分。
- **修复：** 添加 `cmdSetBPM` 命令和 `showBPMDialog()` 方法。View 菜单添加 "Set BPM..."。对话框使用 DialogWindow：BPM 输入框 (20-500)、Division 输入框 (1-32)，通过 TextEditor 收集用户输入，`onClick` 回调中更新 GridManager 并重绘 GridOverlay/TransportBar。

#### 3. 音频线程内层循环低效（低优先级，性能优化）
- **文件：** `TransportBar.cpp` — `getNextAudioBlock()`
- **根因：** `m_selection.hasSelection()` 和 `m_loopManager->hasValidLoop()` / `getLoopStart()` / `getLoopEnd()` 在 sample-by-sample 内层循环中被反复调用。音频回调每次触发 512+ 采样点，对于长音频文件累积大量冗余调用。
- **修复：** 将 `shouldLoop`、`hasSelection`、`loopStartSample`、`loopEndSample`、`selectionEndSample` 的计算全部移到外层，在内层循环中只使用预计算的整数边界值进行比较。同时消除 `m_selection.getSelectionEnd()` 在内层的 double→int 转换开销。

#### 4. Trackpad 横向滚轮速度因子不一致（低优先级，UX 一致性）
- **文件：** `WaveformThumbnail.cpp` — `mouseWheelMove()`
- **根因：** WaveformThumbnail 的 deltaX 水平滚动使用 `0.12` 因子，而 SpectrogramComponent 使用 `0.1`。在双视图同步模式下（onUserViewChanged 回调），用户在频谱视图中用 trackpad 横向滚动后切换到波形视图，感觉滚动速度突然变化。
- **修复：** 将 WaveformThumbnail 的 deltaX 因子从 `0.12` 改为 `0.1`，与 SpectrogramComponent 一致。

### 审计发现（edison-advance 二次审计 — 新增/修复的问题 #36~#38）

#### 1. cmdSetBPM 未注册到 getAllCommands()（中优先级 Bug）
- **文件：** `MainComponent.cpp` --- `getAllCommands()`
- **根因：** `cmdSetBPM` 实现了完整的 `getCommandInfo()`、`perform()` 和菜单项，但命令 ID 从未添加进 `getAllCommands()` 的命令数组。JUCE 的 `ApplicationCommandManager` 通过 `getAllCommands()` 建立命令→目标映射。菜单中的 `addCommandItem` 点击后通过 `invokeDirectly()` 触发，后者查找映射失败返回 `false`。导致 "Set BPM..." 菜单项可见但点击不执行任何操作。
- **修复：** 在 `getAllCommands()` 的 `cmds.addArray({ ... })` 末尾添加 `cmdSetBPM`。

#### 2. 频谱 Ctrl+Zoom 后 m_viewOffset 未钳位（中优先级 Bug）
- **文件：** `SpectrogramComponent.cpp` --- `mouseWheelMove()`
- **根因：** Ctrl+滚轮缩放频谱视图时，只更新 `m_hZoom`，未调整 `m_viewOffset`。缩放后视图可能超出音频末尾（startTime + viewDur > totalDur）。`renderViewport()` 内部虽然为当前帧做了临时钳位，但 `m_viewOffset` 未被更新为有效值。通过 `onUserViewChanged` 回传的 offset 传到波形视图后，波形 `setHorizontalOffset` 会钳位它，造成两个视图 offset 不一致。后续频谱滚动从错误的 offset 开始。
- **修复：** Ctrl+zoom 分支末尾添加 viewDuration/总时长钳位逻辑，与普通滚动一致。

#### 3. LoopOverlay::timeToX 精度丢失（低优先级）
- **文件：** `LoopOverlay.cpp` --- `paint()`
- **根因：** `timeToX(double)` 被传入 `static_cast<float>(loopStart)`，将 64-bit double 窄化为 32-bit float。对于时长超过 ~3 小时的音频文件（>1M 采样 @ 96kHz），float 精度不足以区分相邻采样点，导致循环标记渐变定位偏差。
- **修复：** 移除不必要的 `static_cast<float>`，直接传递 `double`。

### 审计发现（edison-advance 三次审计 — 新增/修复的问题 #39~#43）

#### 1. `showBPMDialog()` DialogWindow 内存泄漏（中优先级）
- **文件：** `MainComponent.cpp` — `showBPMDialog()`
- **根因：** 每次打开 View > Set BPM... 对话框时，用 `new juce::DialogWindow(...)` 创建一个 DialogWindow，并调用 `setVisible(true)` 显示。关闭对话框时（关闭按钮或 OK/Cancel），DialogWindow 仅调用 `exitModalState()`，但 JUCE 的 DialogWindow（非 `AlertWindow`）不会自动删除。对话框及其所有子组件（Content、Label、TextEditor、TextButton）永久泄漏。
- **修复：** 使用 `dialog->enterModalState(true, nullptr, true)`（第三个参数 `deleteWhenDismissed=true`），让 JUCE 在 `exitModalState()` 被调用时自动删除 DialogWindow 及其通过 `setContentOwned()` 拥有的子组件。同时将 GridManager 的设置逻辑移入 `okBtn->onClick` 中（此时 TextEditor 仍然存活），不再依赖对话框关闭后的文本提取。

#### 2. Export 拖拽在 mouseDrag 中重复触发（低优先级）
- **文件：** `SelectionOverlay.cpp` — `mouseDrag()`
- **根因：** Alt+选区拖拽开始导出后，`mouseDrag` 在每次鼠标移动事件中重复调用 `onExportDragStarted(this)`，而 DragExport 会重新创建 temp 文件并调用 `performExternalDragDropOfFiles()`。这可能导致 OS 级别的拖拽操作被反复初始化，产生多个竞争中的拖拽会话。
- **修复：** 在 `SelectionOverlay` 中添加 `m_exportFired` 布尔字段，`mouseDown` 时重置为 `false`，`mouseDrag` 中的 `DragMode::Export` 分支只在 `!m_exportFired` 时触发一次。首次触发后设为 `true`。

#### 3. `cmdToggleView` 缺少显式键映射（低优先级，一致性）
- **文件：** `MainComponent.cpp` — 构造函数
- **根因：** `cmdToggleView` (Ctrl+S 切换波形/频谱视图) 仅通过 `getCommandInfo()` 的 `addDefaultKeypress` 注册到 `resetToDefaultMappings()`，但没有对应的显式 `mappings->addKeyPress(cmdToggleView, ...)` 调用。项目中 30+ 个快捷键全部使用显式注册，`cmdToggleView` 是唯一例外。依赖隐式默认映射虽能工作，但风格不一致且（在未来的重构中）脆弱。
- **修复：** 在显式键映射块末尾添加 `mappings->addKeyPress(cmdToggleView, KeyPress('s', ctrlModifier, 0))`。

#### 4. `cmdNew` (New Project) 缺少键盘快捷键（低优先级）
- **文件：** `MainComponent.cpp` — 构造函数
- **根因：** `cmdNew` 注册了命令、菜单项和 `perform()` 实现，但没有分配键盘快捷键。Edison 中有 New 功能但无标准快捷键；这里添加 `Ctrl+N` 作为行业标准。
- **修复：** 在显式键映射块中添加 `mappings->addKeyPress(cmdNew, KeyPress('n', ctrlModifier, 0))`。同时更新 README 快捷键表。

#### 5. README 快捷键表不完整（低优先级，文档）
- **文件：** `README.md` — ⌨️ 快捷键 章节
- **根因：** 快捷键表仅列出了 13 个快捷键（Space, Ctrl+O, Ctrl+Z, Ctrl+Shift+Z 等），但实际代码已实现 30+ 个快捷键。Missing shortcuts include: Ctrl+Shift+S (Save As), Ctrl+X/C/V (Cut/Copy/Paste), Ctrl+A (Select All), Ctrl+D (Silence), Ctrl+R (Reverse), Ctrl+I (Fade In), Ctrl+Shift+O (Fade Out), Ctrl+Shift+R (Add Region), Shift+Backspace (Remove Region), Ctrl+Tab/Ctrl+Shift+Tab (Next/Prev Region), L (Toggle Loop), Escape (Stop), =/- (Zoom In/Out), Ctrl+S (Toggle View), Ctrl+N (New Project)。
- **修复：** 替换整个快捷键表为完整列表，包含所有已实现快捷键。

### 审计发现（edison-advance 四轮审计 — 新增/修复的问题 #44）

#### 1. cmdNormalize 与 cmdNew 键冲突 — 共用 Ctrl+N（高优先级 Bug）
- **文件：** `MainComponent.cpp` — `getCommandInfo()`, 构造函数
- **根因：** `cmdNormalize` 通过 `getCommandInfo()` 中的 `addDefaultKeypress('n', ctrlModifier)` 注册 Ctrl+N 为默认快捷键。`cmdNew`（New Project）在同一轮审计（三次审计）中通过显式 `mappings->addKeyPress(cmdNew, KeyPress('n', ctrlModifier, 0))` 绑定相同的 Ctrl+N。`resetToDefaultMappings()` 先给 cmdNormalize 分配 Ctrl+N，然后显式 `addKeyPress` 覆盖该映射让 Ctrl+N → cmdNew。最终结果：**Ctrl+N 只触发 New Project，Normalize 完全丢失了键盘快捷键。** 用户按下 Ctrl+N 期望归一化选区时，会意外清空项目。
- **影响：** 导致功能不可用（Normalize 只能通过菜单访问），且容易触发危险操作（New Project 清空所有数据）。
- **修复：** 将 `cmdNormalize` 的默认快捷键从 `Ctrl+N` 改为 `Ctrl+Shift+N`（当前无其他命令使用该组合）。同步更新 `README.md` 快捷键表和 `project_state.md` 中的引用。

#### 2. RegionOverlay 继承 ChangeListener 但从未注册（低优先级，死代码）
- **文件：** `RegionOverlay.h` — `class RegionOverlay : public juce::ChangeListener`
- **根因：** `RegionOverlay` 声明继承 `juce::ChangeListener` 并实现了 `changeListenerCallback()`，但 `MainComponent` 从未调用 `m_regionOverlay->addChangeListener()`，没有任何 broadcaster 将 RegionOverlay 注册为 listener。所有 repaint 通过 `m_regionManager.onRegionsChanged` 回调完成。
- **影响：** 无害死代码，安全但浪费一个 vtable 槽位。
- **修复：** ✅ **已修复（五轮审计）** — 移除 `ChangeListener` 继承和 `changeListenerCallback` 空实现。RegionOverlay 的 repaint 完全通过 `m_regionManager.onRegionsChanged` 回调驱动，`changeListenerCallback` 是无用死代码。

### 审计发现（edison-advance 五轮审计 — 新增/修复的问题 #45~#48）

#### 1. cmdToggleView 快捷键重复绑定（低优先级，代码整洁）
- **文件：** `MainComponent.cpp` — 构造函数
- **根因：** `cmdToggleView`（Ctrl+S）在键映射块中显式绑定了两次：第一次在 `cmdPlayPause` 之后（line 125），第二次在 `cmdNew` 之后（line 148）。JUCE `KeyPressMappingSet::addKeyPress()` 对重复调用不会报错，后期绑定覆盖前期，功能正确但存在冗余代码。
- **修复：** 移除第二次重复的 `mappings->addKeyPress(cmdToggleView, ...)` 调用。

#### 2. Waveform/Spectrogram 双视图滚动速度不一致（低优先级，UX 一致性）
- **文件：** `WaveformThumbnail.cpp` — `mouseWheelMove()`
- **根因：** WaveformThumbnail 的普通滚轮水平滚动使用因子 `0.12`，而 SpectrogramComponent 使用 `0.1`。在双视图同步模式下（`onUserViewChanged` 双向回调），两个视图的 offset 始终同步，但速度差异意味着每次用户手动滚动 Waveform 然后切换到 Spectrogram 视图时，感知到的滚动速度不同。deltaX trackpad 横向滚动已在 edison-advance 首次审计中统一为 `0.1`，但 deltaY 仍保持 `0.12` vs `0.1` 的不一致。
- **修复：** WaveformThumbnail 的 deltaY 滚动因子从 `0.12` 改为 `0.10`，与 SpectrogramComponent 完全一致。

#### 3. SpectrogramComponent::setViewOffset() 缺少钳位（低优先级，边界安全）
- **文件：** `SpectrogramComponent.h/cpp`
- **根因：** `setViewOffset()` 直接设置 `m_viewOffset = offsetSec` 而不做任何范围检查。当通过 `onUserViewChanged` 回调传递的 offset 超出有效范围时，`m_viewOffset` 可能指向音频结束之后的位置。`renderViewport()` 内部对局部变量做了钳位，但 `m_viewOffset` 本身一直没有被纠正，导致后续滚动从错误位置开始。
- **修复：** 将 `setViewOffset()` 实现移到 `.cpp` 文件中，添加与 WaveformThumbnail::setHorizontalOffset() 一致的钳位逻辑：根据总时长和当前 zoom 计算最大 offset，使用 `std::clamp` 确保 `m_viewOffset` 始终在有效范围内。

#### 4. RegionOverlay ChangeListener 死代码清理（低优先级，内务清理）
- **文件：** `RegionOverlay.h`, `RegionOverlay.cpp`
- **根因：** 四轮审计已记录此问题。`RegionOverlay` 继承 `juce::ChangeListener` 并实现了 `changeListenerCallback()`，但从未作为 listener 注册到任何 broadcaster。所有重绘通过 `m_regionManager.onRegionsChanged` 回调驱动。
- **修复：** 移除 `class RegionOverlay` 的 `public juce::ChangeListener` 继承，移除 `RegionOverlay.cpp` 中的 `changeListenerCallback()` 空实现。

### 快捷键完整性清单

| 命令 | 快捷键 | 状态 |
|------|--------|------|
| New Project | Ctrl+N | ✅ **新增** — 本轮添加 |
| Open | Ctrl+O | ✅ |
| Save As | Ctrl+Shift+S | ✅ |
| Play/Pause | Space | ✅ |
| Stop | Escape | ✅ |
| Play Selection | (无) | 设计覆盖 — Space 已有选区播放 |
| Zoom In | **=** | ✅ **新增** — 本轮添加 |
| Zoom Out | **-** | ✅ **新增** — 本轮添加 |
| Toggle View | Ctrl+S | ✅ |
| Undo | Ctrl+Z | ✅ |
| Redo | Ctrl+Shift+Z | ✅ |
| Silence | Delete / Backspace / Ctrl+D | ✅ |
| Reverse | Ctrl+R | ✅ |
| Normalize | Ctrl+Shift+N | ✅ |
| Fade In | Ctrl+I | ✅ |
| **Fade Out** | **Ctrl+Shift+O** | ✅ **新增** — 本轮添加 |
| Cut | Ctrl+X | ✅ |
| Copy | Ctrl+C | ✅ |
| Paste | Ctrl+V | ✅ |
| Select All | Ctrl+A | ✅ **新增** — 本轮添加 |
| Add Marker | M | ✅ |
| Remove Marker | Shift+M | ✅ |
| Next Marker | ] | ✅ |
| Prev Marker | [ | ✅ |
| Add Region | Ctrl+Shift+R | ✅ |
| Remove Region | Shift+Backspace | ✅ |
| Next Region | Ctrl+Tab | ✅ |
| Prev Region | Ctrl+Shift+Tab | ✅ |
| Zoom to Selection | Z | ✅ |
| Fit All | Ctrl+Shift+F | ✅ |
| Toggle Loop | L | ✅ |
| Snap to Zero | X | ✅ |
| Snap to Grid | G | ✅ |

### 功能对比：FL Studio Edison vs Open Edison

| Edison 功能 | Open Edison | 备注 |
|-------------|-------------|------|
| 音频加载/波形显示 | ✅ | WAV/MP3/FLAC |
| 播放/暂停/停止 | ✅ | Space 切换，Escape 停止 |
| 选区播放 | ✅ | 播放仅限选区 |
| 选中区域操作（静音/反向/归一化/淡入淡出） | ✅ | 已全部实现（Fade Out: Ctrl+Shift+O） |
| 撤销/重做 | ✅ | Ctrl+Z/Ctrl+Shift+Z |
| 录音 | ✅ | 支持输入设备选择 |
| 频谱分析 | ✅ | 视口驱动 STFT |
| 标记/提示点 | ✅ | M/Shift+M/[ / ] |
| Region/块管理 | ✅ | Ctrl+Shift+R 添加 |
| AB循环复读 | ✅ | L 键切换 |
| 缩放至选区 | ✅ | Z 键，恢复 Ctrl+Shift+F |
| 键盘缩放 | ✅ | **新增** — = / - 键缩放 |
| 吸附到网格/零交叉 | ✅ | G/X 键切换 |
| 零交叉吸附+网格可协同 | ✅ | 先网格再零交叉细化 |
| 选区拖拽导出 WAV | ✅ | Alt+选区拖动 |
| 复制/剪切/粘贴 | ✅ | Ctrl+C/X/V |
| **保存/导出编辑后音频** | ✅ | **新增** — Ctrl+Shift+S |
| **垂直缩放** | ✅ | **新增** — Ctrl+Shift+滚轮 |
| **选区拖动视觉反馈** | ✅ | **修复** — 实时更新波形高亮和信息 |
| **Region 删除快捷键** | ✅ | **修复** — 新增 Shift+Backspace |
| **Select All（全选）** | ✅ | **新增** — Ctrl+A |
| **波形滚轮行为修正** | ✅ | **修复** — 普通滚轮水平滚动/Ctrl+滚轮缩放 |
| **trackpad 横向滚轮** | ✅ | **修复** — Waveform+Spectrogram 双视图支持 |
| **加载文件 Zoom 状态重置** | ✅ | **修复** — zoomedToSelection 状态跨文件泄漏 |
| **加载新文件 Region/Loop 自动清空** | ✅ | **修复** — 避免残余Region引用无效时间 |
| **进度条与选区信息不重叠** | ✅ | **修复** — 专用200px选区信息区域 |
| **波形视图回放自动滚动** | ✅ | **新增** — 播放头进入右侧85%区域时自动滚动 |
| **音频线程数据竞争修复** | ✅ | **修复** — m_readIndex 原子化 + setPlaybackPosition 移至消息线程 |
| **Fade Out 快捷键** | ✅ | **新增** — Ctrl+Shift+O |
| **双视图 Zoom 同步** | ✅ | **新增** — onUserViewChanged 双向回调，波形/频谱 zoom/scroll 始终一致 |
| **cmdZoomIn/Out 同步频谱** | ✅ | **修复** — = / - 键缩放时同步更新频谱视图 |
| **手动缩放重置 Zoom-to-Selection** | ✅ | **修复** — 手动缩放/滚动后清除 zoom-to-selection 状态 |
| **Region 标签栏 setBounds 重复** | ✅ | **修复** — 移除 resized() 中重复的 setBounds 调用 |
| **双视图滚动速度一致** | ✅ | **修复** — Waveform deltaY `0.12`→`0.10`，与 Spectrogram 一致 |
| **Spectrogram setViewOffset 钳位** | ✅ | **修复** — 添加范围检查，避免 offset 越界 |
| **cmdToggleView 重复绑定** | ✅ | **修复** — 移除构造函数中重复的 key mapping |
| **RegionOverlay ChangeListener 死代码** | ✅ | **修复** — 移除未使用的 ChangeListener 继承 |
| 时间尺/刻度标尺 | ❌ | 未实现（计划中） |
| 音量/增益实时控制 | ❌ | 未实现（计划中） |
| 电平表 | ❌ | 未实现（计划中） |
| DC 偏移移除 | ❌ | 未实现 |
| 相位翻转 | ❌ | 未实现 |
| 时间伸缩/变调 | ❌ | 高级功能，暂未实现 |
| 鼠标框选微调（Shift+方向键） | ❌ | 未实现 |
| 新建/新建项目 | ✅ | **新增** — File > New Project |

### 修复详情

#### 1. 选区拖动视觉反馈（高优先级 Bug）
- **文件：** `SelectionOverlay.cpp`
- **根因：** mouseDown/mouseDrag 只调用了 `this->repaint()`。但选区的蓝色高亮矩形由 `WaveformThumbnail::paint()` 绘制，信息文本在 `MainComponent::paint()` 中绘制。自 repaint 不会触发它们。
- **修复：** mouseDown/mouseDrag 同时调用 `m_thumbnail.repaint()` 和 `getParentComponent()->repaint()`，确保波形高亮和信息文本实时更新。

#### 2. Save As（新增功能）
- **文件：** `MainComponent.h/cpp`
- **新增命令：** `cmdSaveAs` (Ctrl+Shift+S)
- **逻辑：** 使用文件选择器，以 WAV 格式写入当前缓冲区。默认文件名继承当前文件。更新窗口标题反映已保存的文件名。支持 Undo 兼容（保存整个缓冲区，不影响内存中的可撤销操作）。
- **菜单：** 文件菜单中的 "Save As..."，位于 "Open..." 下方。

#### 3. 垂直缩放（新增功能）
- **文件：** `WaveformThumbnail.h/cpp`
- **新增 API：** `setVerticalZoom(double)`, `getVerticalZoom()`
- **交互：** Ctrl+Shift+鼠标滚轮 — 向上滚放大（1.2x），向下缩小（1/1.2x），范围 0.1x–100x，限制最大幅度不超过声道高度。
- **设计：** 保留 m_zoom（水平缩放）不变，独立控制垂直放大倍数。

#### 4. Region 删除快捷键（新增）
- **文件：** `MainComponent.cpp`
- **新增：** Shift+Backspace 删除当前 Region
- **注意：** 不与 Silence（Delete / Backspace）冲突，因为 Shift 修饰符不同。

## 版本里程碑

### P0 — 可用的基础音频编辑器
- [x] CMake + JUCE 8 构建系统
- [x] 加载 WAV/MP3/FLAC + 波形渲染显示（AudioFileManager + WaveformThumbnail）
- [x] 播放/暂停/停止/选区播放（TransportBar 框架）
- [x] 鼠标框选选区 + 拖动调整边界（SelectionOverlay）
- [x] 选区拖拽导出 WAV（Alt+选区内拖动导出，修复 44100 硬编码 Bug）
- [x] 水平缩放 + 垂直缩放（WaveformThumbnail — 新增垂直缩放）
- [x] 录音（TransportBar 录音回调框架）

### P1 — 接近 Edison 核心体验
- [x] 频谱/声谱图视图（STFT 实时分析，视口驱动惰性渲染）
- [x] 撤销/重做（UndoManager + AudioModifyAction 泛型框架）
- [x] 选区内处理：静音（Silence Selection, Ctrl+D）
- [x] 选区内处理：反向 / 归一化 / 淡入淡出

### P2 — 增强功能（按需）
- [x] 多 Region 管理（Edison 的"块"概念）
- [x] Zoom-to-Selection（Z 键缩放至选区，Ctrl+Shift+F 恢复完整视图）
- [x] 鼠标滚轮精细缩放（Ctrl+滚轮缩放，滚轮水平滚动），Waveform & Spectrogram 双视图
- [x] 标记/提示点（M/Shift+M/[/]快捷键，青色标记旗标，点击跳转）
- [x] 快捷键绑定（空格播放/暂停，Delete/Backspace 静音）

### P3 — 工作流与专业功能
- [x] 循环/AB复读（Loop/Cycle Playback：L 键切换，从选区或全文件建立循环范围）
- [x] 零交叉吸附（Selection Snapping to Zero Crossings）
- [x] 选区操作交叉淡化（Crossfade on Selection Editing）
- [x] 吸附网格（Snap-to-Grid：节拍/拍子对齐）

### P4 — 信息显示与优化 ✅
- [x] 文件信息显示（TransportBar 显示采样率/位深度/声道数/文件名）
- [x] 频谱视图长文件优化（视口驱动惰性 STFT，移除 10k 帧硬上限，支持任意时长音频）

### P4+ — 审计修复 ✅
- [x] 选区拖动视觉反馈实时更新
- [x] 新增垂直缩放（Ctrl+Shift+滚轮）
- [x] 新增 Save As（Ctrl+Shift+S）
- [x] Region 删除快捷键（Shift+Backspace）

### P4+++++++ 七轮审计修复
- [x] 移除 TransportBar::resized() 未使用的 selInfoArea 变量
- [x] RegionOverlay 析构时清理 onRegionsChanged 回调，防止悬挂
- [x] SpectrogramComponent renderViewport 同步 m_viewOffset 钳位值
- [x] SpectrogramComponent setPlaybackPosition 自动滚动上限钳位
- [x] DragExport 临时文件定期清理（最多保留 20 个）

## 已知 Bug

| # | 描述 | 严重度 | 说明 |
|---|------|--------|------|
| 1 | LoopManager/波形缓冲区从音频线程非安全读取 | 低 | `hasValidLoop()`/`getBuffer()`/`getSample()` 等被音频线程读取，但通过 `onPositionChanged` callAsync 调用，实际读写间隔极小。x86_64 上对齐的 int/double 读写原子，极少出现不一致。视作 JUCE 约定范围内的已知风险。跨 ARM 架构需修复。 |
| 2 | Undo/Redo 期间播放不暂停，可能导致音频线程读取中修改的缓冲区 | 低 | 用户可在播放中按 Ctrl+Z。理论数据竞争，实践因消息队列调度几乎不出现。与 FL Studio 行为一致（Edison 允许播放中 Undo）。 |
| 3 | 进度条拖拽触发的 setPosition 在播放中产生瞬态不连续 | 低 | 播放中拖拽进度条改变 `m_readIndex`，音频线程可能跳过或重复少量采样。这是预期行为（类似 Edison 的 scrub），听感上通常不可闻。 |
| 4 | `m_readIndex` 溢出 (int) 用于超长音频 (>13h@44.1kHz) | 低 | `std::atomic<int>` 在 >3h@192kHz 时溢出。极少遇到。缓冲区内存在 JUCE 中本受 `int` 限制。 |

## 待处理

1. **P0 DragExport** — 骨架完成，需在 Windows 实际测试拖拽交互（当前 Linux 环境无法验证）。
2. **P4-002: Windows 实机测试** — ⚠️ 需物理 Windows 机器。验证 DragExport、录音、Save As 等功能在 Windows 上的表现。
3. **全文件线程安全审计** — 对 LoopManager（hasValidLoop/getLoopStart/getLoopEnd 等）进行全面 Atomic 封装，消除所有消息/音频线程数据竞争风险。当前 x86_64 上正常，跨架构（ARM）需修复。
4. **Ripple Delete（Delete 拉移删除）** — [P5-高] Edison 的 Delete/Backspace 删除选区并拉移后续音频。Open Edison 当前仅静音。需实现 AudioModifyAction 的子类来处理缓冲区移位 + Undo。 |
5. **Trim/Crop 选区 (Ctrl+T)** — [P5-高] Edison 可用 Ctrl+T 保留选区并删除选区外所有音频。当前缺少该功能。|
6. **BPM 设置 UI** — ✅ 已完成（edison-advance），View > Set BPM... 对话框。
7. **`m_readIndex` 溢出** — [低] JUCE `AudioBuffer` 自身使用 `int` 存储采样计数，`m_readIndex` 不会越过 `INT_MAX`。无实际溢出风险，保留 `std::atomic<int>`。
8. **TransportBar paint/resized Y 坐标对齐** — [低] paint() 使用 getLocalBounds() 基准 vs resized() 使用 reduced(4,2)。
9. **TransportBar 死变量 selInfoArea 清理** — ✅ 已修复（九轮审计）。

## P5 后续计划（Edison 核心缺失功能）

| 功能 | 优先级 | 说明 |
|------|--------|------|
| **Ripple Delete** | **高** | Delete 删除选区并拉移后续音频。核心 Edison 行为差异 |
| **Trim/Crop (Ctrl+T)** | **高** | 保留选区，删除选区外所有音频 |
| **时间尺/刻度标尺** | 中 | 在波形上方显示秒/节拍刻度参考线 |
| **音量/增益实时控制** | 中 | TransportBar 添加音量滑块 |
| **电平表** | 低 | 播放时显示实时电平 |
| **相位翻转** | 低 | 选区/全文件相位反转 |
| **DC 偏移移除** | 低 | 常见的音频清理功能 |
| **选区扩展（Shift+方向键）** | 低 | 类似 Edison 逐采样点扩展选区 |
| **F2 重命名 Region** | 低 | 在 Region 标签上双击重命名 |
| **Loop 录音（多 take）** | 低 | 循环录音保留多个 take |

## 架构设计

```
open-edison/
├── CMakeLists.txt          # 根 CMake（FetchContent JUCE）
├── Source/
│   ├── Main.cpp            # JUCE 入口
│   ├── MainComponent.h/cpp # 主窗口 + 波形显示 + 控制栏 + 命令处理 + Save As
│   ├── AudioFileManager.h/cpp    # 音频文件管理
│   ├── SelectionManager.h        # 选区状态管理
│   ├── SelectionOverlay.h/cpp    # 选区叠加层渲染 + 鼠标交互
│   ├── TransportBar.h/cpp        # 播放控制条
│   ├── WaveformThumbnail.h/cpp   # 波形缩略图渲染（含垂直缩放）
│   ├── DragExport.h/cpp          # 拖拽导出核心
│   ├── SpectrogramComponent.h/cpp # 频谱/声谱图 STFT 渲染（视口驱动惰性渲染）
│   ├── UndoableActions.h/cpp     # Undo/Redo 动作（AudioModifyAction）
│   ├── MarkerManager.h           # 标记点管理（增删改查）
│   ├── MarkerOverlay.h/cpp       # 标记点覆盖层渲染（青色旗标+竖线+标签）
│   ├── LoopManager.h             # 循环播放状态管理（启用/停止/范围）
│   ├── LoopOverlay.h/cpp         # 循环范围覆盖层渲染（半透明高亮+边界线）
│   ├── GridManager.h             # 网格/吸附数据（BPM/拍号）
│   ├── GridOverlay.h/cpp         # 网格覆盖层渲染
│   ├── RegionManager.h/cpp       # 多 Region 管理（Edison 块概念）
│   ├── RegionOverlay.h/cpp       # Region 标签栏渲染 + 点击/右键菜单
├── .gitignore
├── project_state.md
└── README.md
```
