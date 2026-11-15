import '../../src/styles/tokens.css'; import '../../src/styles/foundations.css'; import '../../src/theme.js'; import '../../src/progress.js'; import '../../src/kpi.js'; import './data-visualization.css';
const setSparkline = (id: string, values: number[]) => { const element = document.querySelector(id) as HTMLElement & { sparkline: number[] }; if (element) element.sparkline = values; };
setSparkline('#revenue', [72, 74, 73, 79, 82, 86, 91]); setSparkline('#errors', [26, 23, 25, 19, 18, 15, 12]); setSparkline('#latency', [210, 218, 214, 228, 231, 242, 248]);
