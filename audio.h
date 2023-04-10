#pragma once

float denormal = 1.0e-18f;

void FillAudioBuffer(PBYTE pBuffer) {

	for (int buffercount = 0; buffercount < OUT_BUFFER_SIZE; buffercount += 4) {
		register float outl, outr;

		phase += .125f;
		while (phase > tau) phase -= tau;

		outl = outr = sin(phase) * .5f * amp;

		int aui = (int)outl;	if (aui < -32767) aui = -32767;	else if (aui > 32767) aui = 32767;
		int auj = (int)outr;	if (auj < -32767) auj = -32767;	else if (auj > 32767) auj = 32767;
		unsigned char so0 = aui, so1 = aui >> 8, so2 = auj, so3 = auj >> 8;
		pBuffer[buffercount] = so0;	pBuffer[buffercount + 1] = so1;	pBuffer[buffercount + 2] = so2;	pBuffer[buffercount + 3] = so3;
	}
	denormal = -denormal;

}