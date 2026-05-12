# Open Edison 开发日志

## 开篇

> 2026-05-02 | 决定做一个类似 FL Studio Edison 的轻量桌面音频编辑器

在飞书上和用户讨论后确定了项目定位：基于 JUCE 8 的 C++20 桌面应用，核心功能包括波形编辑、选区操作、拖拽导出 WAV。目标平台 Windows，先在 Linux 上开发。

### 技术选型讨论

- **框架**：JUCE 8（最新稳定版，CMake FetchContent 拉取）
- **语言**：C++20
- **构建**：CMake 3.21+
- **开发方法**：用户通过飞书给出需求，我写 prompt 派发 Codex 子会话写代码
- **Codex 模式**：`--yolo`（绕过沙箱直接写文件）
- **工作流**：每 30 分钟 cron 自动推进一轮

### 任务拆解

将 Edison 的功能分三期：
- **P0**：基础编辑器（加载/播放/选区/导出/录音）
- **P1**：进阶功能（频谱图/撤销/选区处理）
- **P2**：增强功能（标记/Region/缩放适配）
- **P3**：高级功能（循环/零交叉/网格/淡入淡出）

---

## 第一天：2026-05-02

> 从零到 P3 全部实现

### 上午：环境搭建

安装系统依赖（CMake、GCC、libwebkit2gtk、ALSA、JACK 等），配置 CMakeLists.txt 用 FetchContent 拉取 JUCE 8。

### 第一次派发 Codex

子会话带着完整 prompt 去执行，要求创建全部 7 个模块的 P0 骨架。但 Codex 子会话进入 CMake 修复循环，卡了 1 小时 4 分钟。最终 kill 掉。

**教训**：Codex 子会话要给定足够具体的 prompt，且建 CMake 时要保证一次性通过。

### 手动修复编译错误

接手后手动修复了所有编译错误：

1. `AudioFileManager.cpp` — `*m_formatManager` 改为 `m_formatManager`（非指针不可解引用）
2. `MainComponent.cpp` — lambda 捕获变量放入局部副本、`browseForFileToOpen()` 改为 `launchAsync()`（JUCE 8 API 变化）
3. `MainComponent.h` — 添加缺失的 `#include <juce_audio_devices/juce_audio_devices.h>`
4. `TransportBar.cpp` — `clearSample()` 改为 `clear(ch, s, 1)`（JUCE 8 API 变化）
5. `MainComponent.cpp` — `getDeviceName()` 改为 `getName()`（JUCE 8 API 变化）

最终编译通过 ✅，第一次提交为 P0 骨架（14 个源文件，1232 行）。

### Cron 持续推进

配置了 `edison-advance` 定时任务（每 30 分钟），自动从 TODO 取任务、派 Codex、commit、更新文档。这个 cron 跑了多轮，从 P0 一路推到 P3 全部完成。

最终状态：**16 个模块、31 个源文件、3,711 行代码**。

### 定时任务问题修复

第一次 cron 触发时报错 `timeout`——初始超时设了 10 分钟，但 Codex 构建时间长。调整为 60 分钟解决。

第二次 cron 报错 `edit` 工具在 `project_state.md` 上失败——原因是 `edit` 要求 oldText 精确匹配，文件多次更新后匹配不上。把 prompt 改为强制使用 `write` 工具。

---

## 闰期：2026-05-02 ~ 2026-05-11

项目暂停，等待用户 Windows 测试。

期间 Gateway 重启导致 Feishu 插件丢失（`@m1heng-clawd/feishu` 在 OpenClaw 版本升级后被移除）。安装了官方 `@openclaw/feishu` 插件并重启解决。

---

## 2026-05-11

### GitHub 推送

用户配置了 `gh` CLI（账号：`aolianfei12306`），仓库：`https://github.com/aolianfei12306/JUCE-edison`。

首次推送上去了，但 commit 作者显示为 `cjinfdu`（因为初始化时设了 `jc@fudan.edu.cn`，GitHub 自动关联到了 cjinfdu 账号）。

**修复**：用 `git filter-branch` 重写全部 25 个 commit 的作者为 `aolianfei12306 <1659027547@qq.com>`，force push。

### Windows 零浏览器安装

用户提出"不需要手动去任何网页下载任何东西"，于是创建了 `setup.ps1`：

1. 检测 winget（Win10/11 内置）→ 安装 MSYS2 → `pacman` 装 GCC + CMake + Ninja
2. 否则检测 Chocolatey / Scoop
3. 否则自动装 Chocolatey

写入 README 后 push。

---

## 2026-05-12

### 单元测试

创建基于 JUCE UnitTest 的测试框架：

- 6 个测试文件覆盖：AudioFileManager、GridManager、LoopManager、MarkerManager、RegionManager、SelectionManager
- 独立编译目标 `OpenEdison_Tests`
- 用 `runTestsInCategory()` 隔离项目测试与 JUCE 内置测试
- 测试类别：`EdisonCore`（~50 组测试）、`EdisonAudio`（1 组）

**遇到的坑**：
1. `getResults()` → `getResult(int)`（JUCE 8 API 变化）
2. `expectEquals(juce::String, const char*)` 类型冲突 → 改用 `expect(str == juce::String("..."))`
3. 未链接被测试的源文件（`AudioFileManager.cpp`、`RegionManager.cpp`）导致链接错误
4. JUCE 内置的音频格式测试被同时运行导致超时 → 使用唯一测试类别名 `EdisonCore`

### 文档

创建了完整的项目文档体系：
- `ARCHITECTURE.md` — 模块架构、数据流、代码结构
- `CHANGELOG.md` — 完整变更日志
- `TESTING.md` — 测试说明
- `DEVELOPMENT_LOG.md` — 本文件

---

## 待办

- [ ] Windows 实机测试
- [ ] 修复 Windows 上可能出现的编译问题
- [ ] 完善录音回调逻辑
- [ ] 边缘情况测试（超大文件、异常文件格式）
- [ ] 性能优化（大规模波形渲染的流畅度）
