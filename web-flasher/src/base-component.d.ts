export default class BaseComponent extends HTMLElement {
  /** Get a data-* attribute */
  getProp(key: string): string | null;

  /** Set a data-* attribute */
  setProp(key: string, value?: string | null): void;

  /** Emit a custom event that bubbles */
  emit(eventName: string, detail?: any, options?: CustomEventInit): void;

  /** Render template string into shadow root */
  render(templateString: string): void;
}