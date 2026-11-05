# Vault

**Hide an encrypted message inside an ordinary image.** Vault encrypts your text with a password and buries it in the pixels of a PNG — the output looks identical to the original. Anyone can see the picture; only someone with the password can recover what's inside.

It combines two layers:

- **Encryption** — AES‑256‑GCM, with the key derived from your password (PBKDF2‑HMAC‑SHA256, 600,000 iterations). Without the password the data is unreadable *and* tamper‑evident.
- **Steganography** — the encrypted bytes are written into the least‑significant bits of the image's color channels, so the picture looks unchanged to the eye.

The result: a secret that's both unreadable and invisible, carried in a file nobody would look at twice.

---

## Quick example

Hide a message in `cat.png`, producing `cat_secret.png`:

```bash
./VaultBuilder "meet at the pier, midnight" cat.png cat_secret.png
```
```
Vault Builder
Password:
Confirm password:
Success!
New image now at: cat_secret.png
```

Send `cat_secret.png` to someone (it looks like an ordinary cat photo). They recover the message with the same password:

```bash
./VaultRunner cat_secret.png
```
```
Vault Runner
Password:
Decrypted Data: meet at the pier, midnight
```

Wrong password? The decryption's authentication check fails and nothing leaks:

```
Caught Exception: Decryption failed: authentication tag mismatch
```

---

## Building

Vault needs a C++20 compiler, CMake ≥ 3.20, and OpenSSL. Dependencies are declared in `vcpkg.json`.

**With a system OpenSSL** (simplest on Linux):

```bash
sudo apt install libssl-dev
```
```bash
cmake -S . -B build
```
```bash
cmake --build build
```

**With vcpkg** 

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
```
```bash
cmake --build build
```

This produces two executables in `build/`: `VaultBuilder` and `VaultRunner`.

## Usage

**Hide a message** — `VaultBuilder`:

```bash
./VaultBuilder <message> <input-image> <output-image>
```

Reads `<input-image>`, prompts for a password (twice, to confirm), encrypts the message, and writes the result to `<output-image>`. 

**Reveal a message** — `VaultRunner`:

```bash
./VaultRunner <image>
```

Prompts for the password and prints the hidden message. If the password is wrong or the image carries no Vault payload, it fails with an authentication error rather than printing garbage.

## Technical details

**Image format.** Input can be any format stb_image reads (PNG, JPG, BMP, …), but **output is always PNG** — the format must be lossless, or the hidden bits would be destroyed by re‑compression. The alpha channel is preserved exactly; data is only ever written to the R/G/B channels, so transparent images stay transparent.

**Capacity.** Three bits per pixel (one per color channel), minus a 48‑byte header. Roughly: a 1000×1000 image holds up to ~366 KB.

**Cryptography.**
- Key derivation: PBKDF2‑HMAC‑SHA256, 600,000 iterations, 32‑byte key, random 16‑byte salt per message.
- Cipher: AES‑256‑GCM — authenticated encryption, so a wrong password or any tampering is detected and rejected.

## Limitations

- **Keep the output lossless.** If a messaging app or website re‑encodes the PNG to JPEG (or otherwise recompresses it), the hidden data is lost. Send the file as‑is — as a file/attachment, not an inline photo that gets recompressed.
- **Not resistant to steganalysis.** Sequential LSB embedding hides data from the eye, not from statistical detectors. Vault protects *confidentiality* (via encryption); it does not guarantee the *presence* of a message is undetectable.
- **Password strength is on you.** The encryption is only as strong as the password. PBKDF2 slows down guessing, but a weak password is still a weak password.

## Disclaimer

Vault is a personal project for learning and legitimate private communication. Use it lawfully and only on images and messages you have the right to use.

