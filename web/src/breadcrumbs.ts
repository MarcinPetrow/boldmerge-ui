import { BoldMergeBreadcrumbItem, BoldMergeBreadcrumbs } from './components/breadcrumbs.js';
if (!customElements.get('bm-breadcrumb-item')) customElements.define('bm-breadcrumb-item', BoldMergeBreadcrumbItem);
if (!customElements.get('bm-breadcrumbs')) customElements.define('bm-breadcrumbs', BoldMergeBreadcrumbs);
export { BoldMergeBreadcrumbItem, BoldMergeBreadcrumbs };
