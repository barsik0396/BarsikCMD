// ─── Virtual Filesystem ───────────────────────────────────────────────────────
const VFS = {
  '/': { type: 'dir', children: ['home', 'etc', 'var', 'usr', 'tmp'] },
  '/home': { type: 'dir', children: ['user'] },
  '/home/user': {
    type: 'dir',
    children: ['Documents', 'Downloads', 'Projects', 'Music', '.barsikrc', 'notes.txt', 'todo.md']
  },
  '/home/user/Documents': {
    type: 'dir',
    children: ['report_2024.pdf', 'notes_2024.txt', 'cv.docx']
  },
  '/home/user/Downloads': {
    type: 'dir',
    children: ['archive.tar.gz', 'image.png', 'BarsikCMD_2026.3.12.1.tar.gz']
  },
  '/home/user/Projects': {
    type: 'dir',
    children: ['BarsikCMD', 'my-website', 'scripts']
  },
  '/home/user/Projects/BarsikCMD': {
    type: 'dir',
    children: ['src', 'Makefile', 'README.md', 'LICENSE']
  },
  '/home/user/Projects/BarsikCMD/src': {
    type: 'dir',
    children: ['main.c', 'commands.c', 'utils.c', 'parser.c']
  },
  '/home/user/Projects/my-website': {
    type: 'dir',
    children: ['index.html', 'style.css', 'script.js']
  },
  '/home/user/Projects/scripts': {
    type: 'dir',
    children: ['backup.sh', 'deploy.sh', 'cleanup.sh']
  },
  '/home/user/Music': {
    type: 'dir',
    children: ['playlist.m3u', 'track01.mp3', 'track02.mp3']
  },
  '/etc': { type: 'dir', children: ['hosts', 'fstab', 'passwd', 'barsikrc.d'] },
  '/etc/barsikrc.d': { type: 'dir', children: ['aliases.conf', 'theme.conf'] },
  '/var': { type: 'dir', children: ['log', 'tmp', 'cache'] },
  '/var/log': { type: 'dir', children: ['syslog', 'auth.log', 'barsik.log'] },
  '/usr': { type: 'dir', children: ['bin', 'lib', 'share'] },
  '/usr/bin': { type: 'dir', children: ['barsik', 'bash', 'ls', 'cat', 'grep'] },
  '/tmp': { type: 'dir', children: ['session_tmp', 'cache_barsik'] },

  // files
  '/home/user/notes.txt': { type: 'file', size: '1.2K', modified: 'Mar 10 14:22' },
  '/home/user/todo.md': { type: 'file', size: '842B', modified: 'Mar 12 09:15' },
  '/home/user/.barsikrc': { type: 'file', size: '512B', modified: 'Feb 28 11:00' },
  '/home/user/Documents/report_2024.pdf': { type: 'file', size: '2.4M', modified: 'Jan 15 16:30' },
  '/home/user/Documents/notes_2024.txt': { type: 'file', size: '3.1K', modified: 'Mar  5 10:00' },
  '/home/user/Documents/cv.docx': { type: 'file', size: '89K', modified: 'Dec 20 12:00' },
  '/home/user/Downloads/archive.tar.gz': { type: 'file', size: '14M', modified: 'Mar  1 08:45' },
  '/home/user/Downloads/image.png': { type: 'file', size: '4.2M', modified: 'Feb 14 19:20' },
  '/home/user/Downloads/BarsikCMD_2026.3.12.1.tar.gz': { type: 'file', size: '320K', modified: 'Mar 13 00:01' },
  '/home/user/Projects/BarsikCMD/Makefile': { type: 'file', size: '1.8K', modified: 'Mar 11 22:10' },
  '/home/user/Projects/BarsikCMD/README.md': { type: 'file', size: '4.0K', modified: 'Mar 13 00:01' },
  '/home/user/Projects/BarsikCMD/LICENSE': { type: 'file', size: '1.1K', modified: 'Feb  1 10:00' },
  '/home/user/Projects/BarsikCMD/src/main.c': { type: 'file', size: '6.2K', modified: 'Mar 12 23:55' },
  '/home/user/Projects/BarsikCMD/src/commands.c': { type: 'file', size: '18K', modified: 'Mar 12 23:50' },
  '/home/user/Projects/BarsikCMD/src/utils.c': { type: 'file', size: '9.4K', modified: 'Mar 11 18:30' },
  '/home/user/Projects/BarsikCMD/src/parser.c': { type: 'file', size: '7.7K', modified: 'Mar 10 15:00' },
};

const FILE_ICONS = {
  dir: '📁',
  '.c': '📄', '.h': '📄',
  '.md': '📝', '.txt': '📝',
  '.sh': '⚙', '.conf': '⚙',
  '.pdf': '📕', '.docx': '📘',
  '.png': '🖼', '.jpg': '🖼',
  '.mp3': '🎵', '.m3u': '🎵',
  '.tar.gz': '📦', '.gz': '📦', '.zip': '📦',
  '.html': '🌐', '.css': '🌐', '.js': '🌐',
  'Makefile': '⚙',
  'LICENSE': '📜',
  '.barsikrc': '🐱',
  'barsik': '🐱',
};

function fileIcon(name, type) {
  if (type === 'dir') return '📁';
  if (FILE_ICONS[name]) return FILE_ICONS[name];
  const ext = name.match(/(\.[^.]+(?:\.[^.]+)?)$/);
  if (ext && FILE_ICONS[ext[1]]) return FILE_ICONS[ext[1]];
  const ext2 = name.match(/(\.[^.]+)$/);
  if (ext2 && FILE_ICONS[ext2[1]]) return FILE_ICONS[ext2[1]];
  return '📄';
}

// ─── Shell State ──────────────────────────────────────────────────────────────
const shell = {
  cwd: '/home/user',
  prevCwd: '/home/user',
  history: [],
  historyIndex: -1,
  cmdCount: 0,
  limitHit: false,
};

const LIMIT = 3;

// ─── Path helpers ─────────────────────────────────────────────────────────────
function resolvePath(input) {
  if (!input || input === '~') return '/home/user';
  if (input === '-') return shell.prevCwd;
  let p = input.startsWith('/') ? input : shell.cwd + '/' + input;
  // normalize
  const parts = p.split('/').filter(Boolean);
  const stack = [];
  for (const part of parts) {
    if (part === '..') stack.pop();
    else if (part !== '.') stack.push(part);
  }
  return '/' + stack.join('/');
}

function promptStr() {
  var ver = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
  if (ver === '2026.3.1') return 'BarsikCMD&gt; ';
  return '<span class="t-prompt" style="color:var(--yellow);font-weight:bold">BarsikCMD&gt;</span> ';
}

function rebuildChips() {
  var ver = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
  var hints = ver === '2026.3.1'
    ? ['help', 'ver', 'echo Мяу!', 'clear', 'exit']
    : ['help', 'ver', 'echo Мяу!', 'update', 'clear', 'exit'];
  var chipWrap = document.getElementById('hint-chips');
  if (!chipWrap) return;
  chipWrap.innerHTML = '';
  hints.forEach(function(cmd) {
    var chip = document.createElement('span');
    chip.className = 'hint-chip';
    chip.innerHTML = '<span class="chip-prefix">$</span> ' + cmd;
    chip.addEventListener('click', function() {
      var input = document.getElementById('try-input');
      input.value = cmd;
      input.focus();
    });
    chipWrap.appendChild(chip);
  });
}

// ─── Output helpers ───────────────────────────────────────────────────────────
function line(html, cls = '') {
  return `<div class="t-out-line${cls ? ' ' + cls : ''}">${html}</div>`;
}
function err(msg) {
  return line(`<span style="color:var(--red)">✗ ${msg}</span>`);
}
function ok(msg) {
  return line(`<span style="color:var(--green)">✓ ${msg}</span>`);
}
function empty() {
  return `<div style="height:0.35rem"></div>`;
}

// ─── Commands ─────────────────────────────────────────────────────────────────
const COMMANDS = {

  'barsik': (args) => {
    if (!args.length) return COMMANDS['barsik']['--help']([]);
    const sub = args[0];
    const fn = COMMANDS['barsik'][sub];
    if (fn) return fn(args.slice(1));
    return err(`barsik: неизвестная команда '${sub}'. Введи <span style="color:var(--cyan)">barsik --help</span>`);
  },

  // ── barsik hello ──
  'barsik.hello': (args) => {
    return [
      empty(),
      line('Мяу! BarsikCMD готов к работе.'),
      line(`Это браузерная демо-версия. Для полного опыта — <a href="index.html#install" style="color:var(--cyan)">установи настоящую</a>.`),
      empty(),
    ].join('');
  },

  // ── barsik --version ──
  'barsik.--version': () => {
    return [
      empty(),
      line('BarsikCMD 2026.3.12.1 — Linux shell companion'),
      line('Build: Mar 13 2026  |  License: MIT  |  Author: barsik0396'),
      empty(),
    ].join('');
  },

  // ── barsik --help ──
  'barsik.--help': () => {
    const cmds = [
      ['hello',       'Приветствие и проверка'],
      ['--version',   'Версия программы'],
      ['--help',      'Эта справка'],
      ['ls [path]',   'Список файлов'],
      ['cd <path>',   'Сменить директорию'],
      ['find <name>', 'Поиск файлов'],
      ['sysinfo',     'Информация о системе'],
      ['net',         'Сетевые данные'],
      ['proc [name]', 'Список процессов'],
      ['clear',       'Очистить терминал'],
    ];
    const rows = cmds.map(([cmd, desc]) =>
      line(`  barsik ${cmd.padEnd(16)} ${desc}`)
    ).join('');
    return [
      empty(),
      line('BarsikCMD 2026.3.12.1 — доступные команды:'),
      empty(),
      rows,
      empty(),
      line('Подсказка: нажми Tab для автодополнения, стрелки для истории.'),
      empty(),
    ].join('');
  },

  // ── barsik ls ──
  'barsik.ls': (args) => {
    const showHidden = args.includes('-a') || args.includes('-la') || args.includes('-al');
    const longFmt    = args.includes('-l') || args.includes('-la') || args.includes('-al');
    const pathArg    = args.find(a => !a.startsWith('-')) || null;
    const target     = pathArg ? resolvePath(pathArg) : shell.cwd;
    const node       = VFS[target];

    if (!node) return err(`ls: нет такого пути: ${target}`);
    if (node.type === 'file') {
      const f = VFS[target];
      return line(`${target.split('/').pop()}  ${f.size}  ${f.modified}`);
    }

    let children = node.children || [];
    if (!showHidden) children = children.filter(c => !c.startsWith('.'));

    if (!children.length) return line('(пусто)');

    if (longFmt) {
      const rows = children.map(name => {
        const fullPath = target === '/' ? '/' + name : target + '/' + name;
        const child = VFS[fullPath] || { type: 'file', size: '—', modified: '—' };
        const icon = fileIcon(name, child.type);
        const nameColor = child.type === 'dir' ? 'var(--cyan)' : 'var(--text)';
        const sizeStr = (child.size || '—').padStart(6);
        const dateStr = child.modified || '—';
        return line(`  ${dateStr}  ${sizeStr}  ${name}${child.type === 'dir' ? '/' : ''}`);
      });
      return [empty(), ...rows, empty()].join('');
    }

    // grid view
    const cols = 3;
    const rows = [];
    for (let i = 0; i < children.length; i += cols) {
      const chunk = children.slice(i, i + cols);
      const cells = chunk.map(name => {
        const fullPath = target === '/' ? '/' + name : target + '/' + name;
        const child = VFS[fullPath] || { type: 'file' };
        const icon = fileIcon(name, child.type);
        const display = (name + (child.type === 'dir' ? '/' : '')).padEnd(22);
        return display;
      });
      rows.push(line('  ' + cells.join('')));
    }
    return [empty(), ...rows, empty()].join('');
  },

  // ── barsik cd ──
  'barsik.cd': (args) => {
    const pathArg = args[0] || '~';
    const target = resolvePath(pathArg);
    const node = VFS[target];
    if (!node) return err(`cd: нет такой директории: ${target}`);
    if (node.type !== 'dir') return err(`cd: это файл, а не директория: ${target}`);
    shell.prevCwd = shell.cwd;
    shell.cwd = target;
    return ''; // cd silently succeeds
  },

  // ── barsik find ──
  'barsik.find': (args) => {
    const nameArg = args.find(a => !a.startsWith('-'));
    if (!nameArg) return err('find: укажи имя для поиска. Пример: <span style="color:var(--cyan)">barsik find notes</span>');

    const dirArg = args.includes('-d') ? resolvePath(args[args.indexOf('-d') + 1]) : shell.cwd;
    const typeArg = args.includes('-t') ? args[args.indexOf('-t') + 1] : null;
    const pattern = nameArg.replace(/\*/g, '.*').replace(/\?/g, '.');
    const regex = new RegExp(pattern, 'i');

    const results = [];
    for (const path of Object.keys(VFS)) {
      if (!path.startsWith(dirArg)) continue;
      const node = VFS[path];
      if (typeArg === 'f' && node.type !== 'file') continue;
      if (typeArg === 'd' && node.type !== 'dir') continue;
      const name = path.split('/').pop();
      if (regex.test(name)) results.push({ path, node });
    }

    if (!results.length) return [
      empty(),
      line(`Ничего не найдено по запросу: '${nameArg}'`),
      empty(),
    ].join('');

    const rows = results.map(({ path, node }) => {
      return line(`  ${path}`);
    });

    return [empty(), ...rows, line(`  Найдено: ${results.length}`), empty()].join('');
  },

  // ── barsik sysinfo ──
  'barsik.sysinfo': () => {
    return [
      empty(),
      line('BarsikCMD System Info (browser demo)'),
      empty(),
      line('  CPU   : Intel Core i5-12600K  @3.70GHz  (12 cores)'),
      line('  RAM   : 7.2 GB / 16.0 GB used  [████████░░░░░░░░]  45%'),
      line('  Disk  : 124 GB free / 512 GB  [████████████░░░░]  76%'),
      line('  OS    : Ubuntu 24.04 LTS  (kernel 6.8.0-50-generic)'),
      line('  Uptime: 3 days, 7h 22m 14s'),
      line('  Shell : BarsikCMD 2026.3.12.1'),
      line('  User  : user  (uid=1000)'),
      line('  Host  : linux'),
      empty(),
    ].join('');
  },

  // ── barsik net ──
  'barsik.net': (args) => {
    const sub = args[0];
    if (sub === 'ping') {
      const host = args[1] || '8.8.8.8';
      return [
        empty(),
        line(`PING ${host}: 56 bytes of data.`),
        line(`64 bytes from ${host}: icmp_seq=0 ttl=118 time=12.4 ms`),
        line(`64 bytes from ${host}: icmp_seq=1 ttl=118 time=11.9 ms`),
        line(`64 bytes from ${host}: icmp_seq=2 ttl=118 time=13.1 ms`),
        empty(),
        line(`--- ${host} ping statistics ---`),
        line('3 packets: min=11.9ms  avg=12.5ms  max=13.1ms'),
        empty(),
      ].join('');
    }
    return [
      empty(),
      line('Сетевые интерфейсы:'),
      empty(),
      line('  eth0    192.168.1.42   MAC: a4:bb:6d:1c:ee:02   UP'),
      line('  lo      127.0.0.1      MAC: 00:00:00:00:00:00   UP'),
      line('  wlan0   —              MAC: b8:27:eb:9a:c3:11   DOWN'),
      empty(),
      line('  Внешний IP: 93.184.216.34'),
      line('  Подсказка: barsik net ping <host>'),
      empty(),
    ].join('');
  },

  // ── barsik proc ──
  'barsik.proc': (args) => {
    const filter = args.find(a => a !== 'kill' && !a.startsWith('-') && !/^\d+$/.test(a));
    const killIdx = args.indexOf('kill');
    if (killIdx !== -1) {
      const pid = args[killIdx + 1];
      if (!pid) return err('proc kill: укажи PID');
      return [ok(`Процесс ${pid} завершён.`), ''].join('');
    }

    const procs = [
      { pid: 1,    cpu: '0.0', mem: '0.1', name: 'systemd' },
      { pid: 312,  cpu: '0.1', mem: '0.4', name: 'NetworkManager' },
      { pid: 841,  cpu: '1.2', mem: '2.1', name: 'Xorg' },
      { pid: 1024, cpu: '0.3', mem: '1.8', name: 'gnome-shell' },
      { pid: 1337, cpu: '0.0', mem: '0.2', name: 'barsik' },
      { pid: 1842, cpu: '3.4', mem: '4.2', name: 'firefox' },
      { pid: 2048, cpu: '0.5', mem: '0.9', name: 'code' },
      { pid: 2391, cpu: '0.0', mem: '0.1', name: 'bash' },
      { pid: 3105, cpu: '0.8', mem: '0.6', name: 'python3' },
      { pid: 4096, cpu: '0.1', mem: '0.3', name: 'sshd' },
    ];

    const shown = filter ? procs.filter(p => p.name.includes(filter)) : procs;
    if (!shown.length) return [
      empty(),
      line(`Процессы не найдены: '${filter}'`),
      empty(),
    ].join('');

    const header = line(`  ${'PID'.padEnd(7)}${'CPU%'.padEnd(8)}${'MEM%'.padEnd(8)}NAME`);
    const rows = shown.map(p => {
      return line(`  ${String(p.pid).padEnd(7)}${p.cpu.padEnd(8)}${p.mem.padEnd(8)}${p.name}`);
    });

    return [empty(), header, ...rows, empty(), line(`  Показано: ${shown.length} / ${procs.length}  |  barsik proc kill <pid> — завершить`), empty()].join('');
  },


  // ── echo ──
  'barsik.echo': (args) => {
    return line(args.join(' '));
  },

  // ── exit ──
  'barsik.exit': () => {
    const ver = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
    if (ver === '2026.3.1') {
      return [empty(), line('Пока!'), empty()].join('');
    }
    return [
      empty(),
      line(`<span style="color:var(--red);font-weight:bold">Пока!</span>`),
      line(`<span style="color:var(--text3)">// В браузерной версии выход недоступен — попробуй настоящую: <a href="index.html#install" style="color:var(--cyan)">установить</a></span>`),
      empty(),
    ].join('');
  },

  // ── ver ──
  'barsik.ver': () => {
    // переопределяется в try.html
    return [empty(), line('BarsikCMD версия 2026.3.2'), empty()].join('');
  },

  // ── help ──
  'barsik.help': () => {
    const ver = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
    if (ver === '2026.3.1') {
      return [
        empty(),
        line('Доступные команды:'),
        line('  clear'),
        line('  echo'),
        line('  exit'),
        line('  help'),
        line('  ver'),
        empty(),
      ].join('');
    }
    const sep = line(`<span style="color:var(--cyan)">─────────────────────────────────</span>`);
    const cmds = [
      ['clear',  'очистить экран'],
      ['echo',   'вывести текст на экран'],
      ['exit',   'выйти из BarsikCMD'],
      ['help',   'показать список команд'],
      ['update', 'проверить и установить обновления'],
      ['ver',    'показать версию BarsikCMD'],
    ];
    const rows = cmds.map(([cmd, desc]) =>
      line(`  <span style="color:var(--green);font-weight:bold">${cmd}</span> — ${desc}`)
    ).join('');
    return [
      empty(),
      line(`<span style="color:var(--yellow);font-weight:bold">BarsikCMD — доступные команды:</span>`),
      sep, rows, sep,
      empty(),
    ].join('');
  },


  // ── update ──
  'update': () => {
    return [
      empty(),
      line(`<span style="color:var(--cyan)">Проверка обновлений...</span>`),
      line(`<span style="color:var(--green);font-weight:bold">Обновлений нет. У вас актуальная версия.</span>`),
      empty(),
    ].join('');
  },

  // ── clear ──
  'clear': () => '__CLEAR__',
};

// ─── Autocomplete ─────────────────────────────────────────────────────────────
const ALL_CMDS = ['echo', 'exit', 'ver', 'help', 'update', 'clear'];

function autocomplete(input) {
  const parts = input.trim().split(/\s+/);

  // path completion for cd/ls/find
  if ((parts[0] === 'barsik') && ['cd', 'ls', 'find'].includes(parts[1]) && parts.length >= 3) {
    const partial = parts[parts.length - 1];
    const dir = partial.includes('/') ? resolvePath(partial.substring(0, partial.lastIndexOf('/'))) : shell.cwd;
    const prefix = partial.includes('/') ? partial.substring(0, partial.lastIndexOf('/') + 1) : '';
    const stem = partial.includes('/') ? partial.substring(partial.lastIndexOf('/') + 1) : partial;
    const node = VFS[dir];
    if (node && node.children) {
      const matches = node.children.filter(c => c.startsWith(stem));
      if (matches.length === 1) {
        parts[parts.length - 1] = prefix + matches[0];
        return parts.join(' ');
      }
    }
    return input;
  }

  // command completion
  const matches = ALL_CMDS.filter(c => c.startsWith(input));
  if (matches.length === 1) return matches[0];
  return input;
}

// ─── Execute ──────────────────────────────────────────────────────────────────
function execute(raw) {
  const input = raw.trim();
  if (!input) return '';

  const parts = input.split(/\s+/);
  const cmd = parts[0];
  const args = parts.slice(1);

  if (cmd === 'clear' || (cmd === 'barsik' && args[0] === 'clear')) return '__CLEAR__';

  if (cmd === 'barsik') {
    const sub = args[0];
    if (!sub) return COMMANDS['barsik.--help']([]);
    const fn = COMMANDS[`barsik.${sub}`];
    if (fn) return fn(args.slice(1));
    const _ver1 = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
    return line(_ver1 !== '2026.3.1'
      ? `<span style="color:var(--red)">Неизвестная команда: ${cmd} ${sub}. Введи 'help'.</span>`
      : `Неизвестная команда: ${cmd} ${sub}. Введи 'help'.`);
  }

  if (cmd === 'echo') {
    return line(args.join(' '));
  }

  if (cmd === 'exit')   return COMMANDS['barsik.exit']([]);
  if (cmd === 'ver')    return COMMANDS['barsik.ver']([]);
  if (cmd === 'help')   return COMMANDS['barsik.help']([]);
  if (cmd === 'update') {
    const ver = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
    if (ver === '2026.3.1') {
      return line("Неизвестная команда: update. Введи 'help'.");
    }
    return COMMANDS['update']([]);
  }

  // unknown top-level
  const _ver2 = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
  return line(_ver2 !== '2026.3.1'
    ? `<span style="color:var(--red)">Неизвестная команда: ${cmd}. Введи 'help'.</span>`
    : `Неизвестная команда: ${cmd}. Введи 'help'.`);
}

// ─── Terminal UI ──────────────────────────────────────────────────────────────
function initTerminal() {
  const output   = document.getElementById('try-output');
  const inputEl  = document.getElementById('try-input');
  const promptEl = document.getElementById('try-prompt');
  const modal    = document.getElementById('limit-modal');

  function updatePrompt() {
    promptEl.innerHTML = promptStr();
  }

  function scrollBottom() {
    const body = document.getElementById('try-body');
    body.scrollTop = body.scrollHeight;
  }

  function appendInputLine(raw) {
    const div = document.createElement('div');
    div.className = 't-line';
    div.innerHTML = `${promptStr()} <span class="t-cmd">${escapeHtml(raw)}</span>`;
    output.appendChild(div);
  }

  function appendOutput(html) {
    if (!html) return;
    const div = document.createElement('div');
    div.innerHTML = html;
    output.appendChild(div);
  }

  function escapeHtml(s) {
    return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
  }

  function showModal() {
    modal.classList.add('visible');
  }

  function run(raw) {
    if (!raw.trim()) { scrollBottom(); return; }

    // history
    shell.history.unshift(raw);
    shell.historyIndex = -1;

    appendInputLine(raw);

    // limit check BEFORE running
    if (!shell.limitHit && shell.cmdCount >= LIMIT) {
      shell.limitHit = true;
      const hint = document.createElement('div');
      hint.style.cssText = 'padding:4px 0';
      hint.innerHTML = `<span style="color:var(--yellow)">🐱 Лимит демо: ${LIMIT} команды исчерпаны. Скачай BarsikCMD для полного доступа!</span>`;
      output.appendChild(hint);
      showModal();
      updatePrompt();
      scrollBottom();
      return;
    }

    const result = execute(raw);
    shell.cmdCount++;

    if (result === '__CLEAR__') {
      output.innerHTML = '';
    } else {
      appendOutput(result);
    }

    // show modal after N-th command (non-blocking)
    if (!shell.limitHit && shell.cmdCount >= LIMIT) {
      shell.limitHit = true;
      setTimeout(() => {
        const hint = document.createElement('div');
        hint.style.cssText = 'padding:8px 0 4px';
        hint.innerHTML = `<span style="color:var(--yellow)">🐱 Лимит демо: ${LIMIT} команды — попробовал вкус? Скачай BarsikCMD!</span>`;
        output.appendChild(hint);
        showModal();
        scrollBottom();
      }, 150);
    }

    updatePrompt();
    scrollBottom();
  }

  // counter badge
  function updateCounter() {
    const badge = document.getElementById('cmd-counter');
    if (!badge) return;
    const left = Math.max(0, LIMIT - shell.cmdCount);
    if (shell.limitHit) {
      badge.textContent = '∞ свободно';
      badge.style.color = 'var(--green)';
    } else {
      badge.textContent = `${left} из ${LIMIT} команд`;
      badge.style.color = left <= 1 ? 'var(--yellow)' : 'var(--text3)';
    }
  }

  // ── input events ──
  inputEl.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
      const val = inputEl.value;
      inputEl.value = '';
      run(val);
      updateCounter();
    } else if (e.key === 'Tab') {
      e.preventDefault();
      inputEl.value = autocomplete(inputEl.value);
    } else if (e.key === 'ArrowUp') {
      e.preventDefault();
      if (shell.historyIndex < shell.history.length - 1) {
        shell.historyIndex++;
        inputEl.value = shell.history[shell.historyIndex];
      }
    } else if (e.key === 'ArrowDown') {
      e.preventDefault();
      if (shell.historyIndex > 0) {
        shell.historyIndex--;
        inputEl.value = shell.history[shell.historyIndex];
      } else {
        shell.historyIndex = -1;
        inputEl.value = '';
      }
    }
  });

  // click anywhere on terminal body = focus input
  document.getElementById('try-body').addEventListener('click', () => inputEl.focus());

  // modal buttons
  document.getElementById('modal-download').addEventListener('click', () => {
    window.location.href = 'index.html#install';
  });
  document.getElementById('modal-continue').addEventListener('click', () => {
    shell.limitHit = false;
    shell.cmdCount = 0;
    modal.classList.remove('visible');
    updateCounter();
    inputEl.focus();
  });

  // startup greeting
  setTimeout(function() {
    var ver = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
    var greet = (typeof TRY_GREET !== 'undefined') ? TRY_GREET[ver] : "BarsikCMD v2026.3.2 — введи 'help' для списка команд";
    var out = document.getElementById('try-output');
    out.innerHTML = '<div style="height:0.5rem"></div>';
    var div = document.createElement('div');
    if (ver === '2026.3.1') {
      div.innerHTML = '<div style="padding:2px 0">' + greet + '</div><div style="height:0.35rem"></div>';
    } else {
      div.innerHTML = '<div style="color:var(--yellow);font-weight:bold;padding:2px 0">' + greet + '</div><div style="height:0.35rem"></div>';
    }
    out.appendChild(div);
    rebuildChips();
    // also define showGreeting for resetTerminal
    window.showGreeting = function() {
      rebuildChips();
      var v = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
      var g = (typeof TRY_GREET !== 'undefined') ? TRY_GREET[v] : "BarsikCMD v2026.3.2 — введи 'help' для списка команд";
      var o = document.getElementById('try-output');
      o.innerHTML = '<div style="height:0.5rem"></div>';
      var d = document.createElement('div');
      if (v === '2026.3.1') {
        d.innerHTML = '<div style="padding:2px 0">' + g + '</div><div style="height:0.35rem"></div>';
      } else {
        d.innerHTML = '<div style="color:var(--yellow);font-weight:bold;padding:2px 0">' + g + '</div><div style="height:0.35rem"></div>';
      }
      o.appendChild(d);
    };
    // patch ver command
    COMMANDS['barsik.ver'] = function() {
      var v = (typeof currentTryVersion !== 'undefined') ? currentTryVersion : '2026.3.2';
      var verStr = (typeof TRY_VERSIONS !== 'undefined') ? TRY_VERSIONS[v] : 'BarsikCMD версия 2026.3.2';
      if (v === '2026.3.1') {
        return '<div style="height:0.35rem"></div><div class="t-out-line">' + verStr + '</div><div style="height:0.35rem"></div>';
      }
      return '<div style="height:0.35rem"></div><div class="t-out-line"><span style="color:var(--cyan);font-weight:bold">' + verStr + '</span></div><div style="height:0.35rem"></div>';
    };
  }, 0);

  updatePrompt();
  updateCounter();
  inputEl.focus();
}

document.addEventListener('DOMContentLoaded', initTerminal);