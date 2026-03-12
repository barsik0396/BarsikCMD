// ===== CONFIG =====
// Укажи ссылку на основной сайт здесь:
const MAIN_SITE_URL = 'https://example.com'; // <-- замени на реальный URL

// ===== MAIN SITE LINK =====
document.querySelectorAll('#mainSiteLink, #mainSiteLinkMobile').forEach(el => {
  el.href = MAIN_SITE_URL;
});

// ===== MOBILE MENU =====
const burger = document.getElementById('burger');
const mobileMenu = document.getElementById('mobileMenu');
if (burger && mobileMenu) {
  burger.addEventListener('click', () => {
    mobileMenu.classList.toggle('open');
  });
}

// ===== COPY BUTTONS =====
document.querySelectorAll('.copy-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    const targetId = btn.dataset.target;
    const el = document.getElementById(targetId);
    if (!el) return;
    navigator.clipboard.writeText(el.textContent.trim()).then(() => {
      btn.textContent = 'Скопировано!';
      btn.classList.add('copied');
      setTimeout(() => {
        btn.textContent = 'Копировать';
        btn.classList.remove('copied');
      }, 2000);
    });
  });
});

// ===== SIDEBAR ACTIVE ON SCROLL (docs page) =====
const sidebarLinks = document.querySelectorAll('.sidebar-link');
if (sidebarLinks.length > 0) {
  const sections = Array.from(document.querySelectorAll('.doc-section'));
  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        const id = entry.target.id;
        sidebarLinks.forEach(link => {
          link.classList.toggle('active', link.getAttribute('href') === `#${id}`);
        });
      }
    });
  }, { rootMargin: '-20% 0px -70% 0px' });
  sections.forEach(s => observer.observe(s));
}

// ===== HERO TERMINAL TYPEWRITER =====
const cmdEl = document.getElementById('typedCmd');
const outputEl = document.getElementById('termOutput');
if (cmdEl && outputEl) {
  const sequences = [
    { cmd: 'barsik --help', output: 'Показать все доступные команды...' },
    { cmd: 'barsik version', output: 'BarsikCMD v0.1.0 🐱' },
    { cmd: 'barsik run', output: 'Запуск... мурр ✓' },
  ];
  let seqIndex = 0;

  function typeSequence(seq) {
    outputEl.textContent = '';
    cmdEl.textContent = '';
    let i = 0;
    const typer = setInterval(() => {
      cmdEl.textContent += seq.cmd[i];
      i++;
      if (i >= seq.cmd.length) {
        clearInterval(typer);
        setTimeout(() => {
          outputEl.textContent = seq.output;
          setTimeout(() => {
            seqIndex = (seqIndex + 1) % sequences.length;
            typeSequence(sequences[seqIndex]);
          }, 2500);
        }, 400);
      }
    }, 60);
  }

  setTimeout(() => typeSequence(sequences[0]), 600);
}