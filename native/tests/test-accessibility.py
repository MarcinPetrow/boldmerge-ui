#!/usr/bin/env python3

import os
import signal
import subprocess
import sys
import time


FRAME_TITLE = "BoldMerge Native Component Catalog"


def load_atspi():
  try:
    import gi
    gi.require_version("Atspi", "2.0")
    from gi.repository import Atspi
    return Atspi
  except (ImportError, ValueError) as error:
    print(f"AT-SPI introspection unavailable: {error}")
    raise SystemExit(77) from error


def children(node):
  try:
    for index in range(node.get_child_count()):
      child = node.get_child_at_index(index)
      if child is not None:
        yield child
  except Exception:
    return


def walk(node):
  yield node
  for child in children(node):
    yield from walk(child)


def name_of(node):
  try:
    return node.get_name() or ""
  except Exception:
    return ""


def role_of(node):
  try:
    return node.get_role_name()
  except Exception:
    return ""


def find_ready_tree(atspi, timeout=12.0):
  deadline = time.monotonic() + timeout
  while time.monotonic() < deadline:
    desktop = atspi.get_desktop(0)
    for application in children(desktop):
      for node in children(application):
        if role_of(node) == "frame" and name_of(node) == FRAME_TITLE:
          nodes = list(walk(node))
          if any(role_of(item) == "button" and
                 name_of(item) == "Theme: Light" for item in nodes):
            return node, nodes
    time.sleep(0.1)
  return None, None


def require_node(nodes, role, name):
  for node in nodes:
    if role_of(node) == role and name_of(node) == name:
      return node
  raise AssertionError(f"missing accessible node: role={role!r}, name={name!r}")


def main():
  if len(sys.argv) != 2:
    print("usage: test-accessibility.py DEMO", file=sys.stderr)
    return 2
  if not os.environ.get("DISPLAY") and not os.environ.get("WAYLAND_DISPLAY"):
    print("No graphical display is available; AT-SPI test skipped")
    return 77

  atspi = load_atspi()
  environment = os.environ.copy()
  environment["GTK_A11Y"] = "atspi"
  process = subprocess.Popen(
    [sys.argv[1]],
    env=environment,
    stdout=subprocess.DEVNULL,
    stderr=subprocess.PIPE,
    text=True,
  )
  try:
    frame, nodes = find_ready_tree(atspi)
    if frame is None:
      stderr = process.stderr.read() if process.poll() is not None else ""
      raise AssertionError(f"catalog tree did not become ready in AT-SPI: {stderr}")
    require_node(nodes, "button", "Theme: Light")
    require_node(nodes, "button", "Projects")
    require_node(nodes, "progress bar", "Storage used")
    require_node(nodes, "grouping", "Weekly recurring revenue")
    require_node(nodes, "label", "Actual revenue rises from $42K to $68K and remains above forecast after Tuesday.")
    require_node(nodes, "label", "Monday")
    required_input = require_node(nodes, "text", "Workspace name *")
    states = required_input.get_state_set()
    if not states.contains(atspi.StateType.REQUIRED):
      raise AssertionError("Workspace name is not exposed as required")
    if not states.contains(atspi.StateType.INVALID_ENTRY):
      raise AssertionError("invalid Workspace name state is not exposed")
    print(f"AT-SPI catalog tree verified: {len(nodes)} accessible nodes")
    return 0
  finally:
    if process.poll() is None:
      process.send_signal(signal.SIGTERM)
      try:
        process.wait(timeout=5)
      except subprocess.TimeoutExpired:
        process.kill()
        process.wait(timeout=5)


if __name__ == "__main__":
  try:
    raise SystemExit(main())
  except AssertionError as error:
    print(f"AT-SPI assertion failed: {error}", file=sys.stderr)
    raise SystemExit(1) from error
