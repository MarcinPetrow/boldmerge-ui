# Tooltip

Import `@boldmerge/web/tooltip` to register only `<bm-tooltip>`.

```html
<bm-tooltip content="Create a new project">
  <button type="button">Create</button>
</bm-tooltip>
```

The default slot accepts exactly one focusable trigger. Tooltip text comes from
the `content` property/attribute or the `content` slot. Properties are `open`
and `placement` (`auto`, `top`, `bottom`, `left`, or `right`). Public methods
are `show(originalEvent?)` and `hide(originalEvent?)`.

Parts are `tooltip` and `pointer`. `bm-tooltip-open-change` is a bubbling,
composed event with `{ open, originalEvent? }` detail.

Mouse and pen hover use the approved 400ms delay; keyboard focus opens
immediately. Pointer leave, blur, and Escape close the tooltip. Auto placement
uses available viewport space and follows scroll/resize. Touch taps do not open
a sticky tooltip.

Content is descriptive and must not contain interactive controls. When the
trigger has no `aria-description`, the component mirrors its plain-text tooltip
content there and restores it when disconnected. For a custom-element trigger
with open Shadow DOM, it targets the first focusable native descendant.
Consumers may provide their own accessible description, which the component
never overwrites.
