let lockCount = 0;
let previousOverflow = '';

export function lockDocumentScroll(): void {
  if (lockCount === 0) {
    previousOverflow = document.documentElement.style.overflow;
    document.documentElement.style.overflow = 'hidden';
  }
  lockCount += 1;
}

export function unlockDocumentScroll(): void {
  if (lockCount === 0) return;
  lockCount -= 1;
  if (lockCount === 0) document.documentElement.style.overflow = previousOverflow;
}
