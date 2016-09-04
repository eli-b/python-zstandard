/**
 * Copyright (c) 2016-present, Gregory Szorc
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

/* A Python C extension for Zstandard. */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define ZSTD_STATIC_LINKING_ONLY
#define ZDICT_STATIC_LINKING_ONLY
#include "zstd/common/mem.h"
#include "zstd.h"
#include "zstd/dictBuilder/zdict.h"

PyDoc_STRVAR(CompressionParameters__doc__,
"CompressionParameters: low-level control over zstd compression");

typedef struct {
	PyObject_HEAD
	unsigned windowLog;
	unsigned chainLog;
	unsigned hashLog;
	unsigned searchLog;
	unsigned searchLength;
	unsigned targetLength;
	ZSTD_strategy strategy;
} CompressionParametersObject;

static PyObject* CompressionParameters_new(PyTypeObject* subtype, PyObject* args, PyObject* kwargs) {
	CompressionParametersObject* self;
	unsigned windowLog;
	unsigned chainLog;
	unsigned hashLog;
	unsigned searchLog;
	unsigned searchLength;
	unsigned targetLength;
	unsigned strategy;

	if (!PyArg_ParseTuple(args, "IIIIIII", &windowLog, &chainLog, &hashLog, &searchLog,
		&searchLength, &targetLength, &strategy)) {
		return NULL;
	}

	if (windowLog < ZSTD_WINDOWLOG_MIN || windowLog > ZSTD_WINDOWLOG_MAX) {
		PyErr_SetString(PyExc_ValueError, "invalid window log value");
		return NULL;
	}

	if (chainLog < ZSTD_CHAINLOG_MIN || chainLog > ZSTD_CHAINLOG_MAX) {
		PyErr_SetString(PyExc_ValueError, "invalid chain log value");
		return NULL;
	}

	if (hashLog < ZSTD_HASHLOG_MIN || hashLog > ZSTD_HASHLOG_MAX) {
		PyErr_SetString(PyExc_ValueError, "invalid hash log value");
		return NULL;
	}

	if (searchLog < ZSTD_SEARCHLOG_MIN || searchLog > ZSTD_SEARCHLOG_MAX) {
		PyErr_SetString(PyExc_ValueError, "invalid search log value");
		return NULL;
	}

	if (searchLength < ZSTD_SEARCHLENGTH_MIN || searchLength > ZSTD_SEARCHLENGTH_MAX) {
		PyErr_SetString(PyExc_ValueError, "invalid search length value");
		return NULL;
	}

	if (targetLength < ZSTD_TARGETLENGTH_MIN || targetLength > ZSTD_TARGETLENGTH_MAX) {
		PyErr_SetString(PyExc_ValueError, "invalid target length value");
		return NULL;
	}

	if (strategy < ZSTD_fast || strategy > ZSTD_btopt) {
		PyErr_SetString(PyExc_ValueError, "invalid strategy value");
		return NULL;
	}

	self = (CompressionParametersObject*)subtype->tp_alloc(subtype, 1);
	if (!self) {
		return NULL;
	}

	self->windowLog = windowLog;
	self->chainLog = chainLog;
	self->hashLog = hashLog;
	self->searchLog = searchLog;
	self->searchLength = searchLength;
	self->targetLength = targetLength;
	self->strategy = strategy;

	return (PyObject*)self;
}

static void CompressionParameters_dealloc(PyObject* self) {
	PyObject_Del(self);
}

static Py_ssize_t CompressionParameters_length(PyObject* self) {
	return 7;
};

static PyObject* CompressionParameters_item(PyObject* o, Py_ssize_t i) {
	CompressionParametersObject* self = (CompressionParametersObject*)o;

	switch (i) {
	case 0:
		return PyLong_FromLong(self->windowLog);
	case 1:
		return PyLong_FromLong(self->chainLog);
	case 2:
		return PyLong_FromLong(self->hashLog);
	case 3:
		return PyLong_FromLong(self->searchLog);
	case 4:
		return PyLong_FromLong(self->searchLength);
	case 5:
		return PyLong_FromLong(self->targetLength);
	case 6:
		return PyLong_FromLong(self->strategy);
	default:
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}
}

static PySequenceMethods CompressionParameters_sq = {
	CompressionParameters_length, /* sq_length */
	0,							  /* sq_concat */
	0,                            /* sq_repeat */
	CompressionParameters_item,   /* sq_item */
	0,                            /* sq_ass_item */
	0,                            /* sq_contains */
	0,                            /* sq_inplace_concat */
	0                             /* sq_inplace_repeat */
};

PyTypeObject CompressionParametersType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"CompressionParameters", /* tp_name */
	sizeof(CompressionParametersObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)CompressionParameters_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	&CompressionParameters_sq, /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	CompressionParameters__doc__, /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	CompressionParameters_new, /* tp_new */
};

PyDoc_STRVAR(FrameParameters__doc__,
	"FrameParameters: low-level control over zstd framing");

typedef struct {
	PyObject_HEAD
	unsigned contentSizeFlag;
	unsigned checksumFlag;
	unsigned noDictIDFlag;
} FrameParametersObject;

static PyObject* FrameParameters_new(PyTypeObject* subtype, PyObject* args, PyObject* kwargs) {
	FrameParametersObject* self;
	unsigned contentSizeFlag;
	unsigned checksumFlag;
	unsigned noDictIDFlag;

	if (!PyArg_ParseTuple(args, "III", &contentSizeFlag, &checksumFlag, &noDictIDFlag)) {
		return NULL;
	}

	self = (FrameParametersObject*)subtype->tp_alloc(subtype, 1);
	if (!self) {
		return NULL;
	}

	self->contentSizeFlag = contentSizeFlag;
	self->checksumFlag = checksumFlag;
	self->noDictIDFlag = noDictIDFlag;

	return (PyObject*)self;
}

static void FrameParameters_dealloc(PyObject* self) {
	PyObject_Del(self);
}

static Py_ssize_t FrameParameters_length(PyObject* self) {
	return 3;
};

static PyObject* FrameParameters_item(PyObject* o, Py_ssize_t i) {
	FrameParametersObject* self = (FrameParametersObject*)o;

	switch (i) {
	case 0:
		return PyLong_FromLong(self->contentSizeFlag);
	case 1:
		return PyLong_FromLong(self->checksumFlag);
	case 2:
		return PyLong_FromLong(self->noDictIDFlag);
	default:
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}
}

static PySequenceMethods FrameParameters_sq = {
	FrameParameters_length, /* sq_length */
	0,					    /* sq_concat */
	0,                      /* sq_repeat */
	FrameParameters_item,   /* sq_item */
	0,                      /* sq_ass_item */
	0,                      /* sq_contains */
	0,                      /* sq_inplace_concat */
	0                       /* sq_inplace_repeat */
};

PyTypeObject FrameParametersType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"FrameParameters", /* tp_name */
	sizeof(FrameParametersObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)FrameParameters_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	&FrameParameters_sq,       /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	FrameParameters__doc__,    /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	FrameParameters_new,      /* tp_new */
};

PyDoc_STRVAR(DictParameters__doc__,
	"DictParameters: low-level control over dictionary generation");

typedef struct {
	PyObject_HEAD
	unsigned selectivityLevel;
	int compressionLevel;
	unsigned notificationLevel;
	unsigned dictID;
} DictParametersObject;

static PyObject* DictParameters_new(PyTypeObject* subtype, PyObject* args, PyObject* kwargs) {
	DictParametersObject* self;
	unsigned selectivityLevel;
	int compressionLevel;
	unsigned notificationLevel;
	unsigned dictID;

	if (!PyArg_ParseTuple(args, "IiII", &selectivityLevel, &compressionLevel,
		&notificationLevel, &dictID)) {
		return NULL;
	}

	self = (DictParametersObject*)subtype->tp_alloc(subtype, 1);
	if (!self) {
		return NULL;
	}

	self->selectivityLevel = selectivityLevel;
	self->compressionLevel = compressionLevel;
	self->notificationLevel = notificationLevel;
	self->dictID = dictID;

	return (PyObject*)self;
}

static void DictParameters_dealloc(PyObject* self) {
	PyObject_Del(self);
}

static Py_ssize_t DictParameters_length(PyObject* self) {
	return 4;
};

static PyObject* DictParameters_item(PyObject* o, Py_ssize_t i) {
	DictParametersObject* self = (DictParametersObject*)o;

	switch (i) {
	case 0:
		return PyLong_FromLong(self->selectivityLevel);
	case 1:
		return PyLong_FromLong(self->compressionLevel);
	case 2:
		return PyLong_FromLong(self->notificationLevel);
	case 3:
		return PyLong_FromLong(self->dictID);
	default:
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}
}

static PySequenceMethods DictParameters_sq = {
	DictParameters_length, /* sq_length */
	0,	                   /* sq_concat */
	0,                     /* sq_repeat */
	DictParameters_item,   /* sq_item */
	0,                     /* sq_ass_item */
	0,                     /* sq_contains */
	0,                     /* sq_inplace_concat */
	0                      /* sq_inplace_repeat */
};

PyTypeObject DictParametersType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"DictParameters", /* tp_name */
	sizeof(DictParametersObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)DictParameters_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	&DictParameters_sq,        /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	DictParameters__doc__,     /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	DictParameters_new,        /* tp_new */
};


static PyObject *ZstdError;

static inline void ztopy_compression_parameters(CompressionParametersObject* params, ZSTD_compressionParameters* zparams) {
	zparams->windowLog = params->windowLog;
	zparams->chainLog = params->chainLog;
	zparams->hashLog = params->hashLog;
	zparams->searchLog = params->searchLog;
	zparams->searchLength = params->searchLength;
	zparams->targetLength = params->targetLength;
	zparams->strategy = params->strategy;
}

PyDoc_STRVAR(compress__doc__,
"compress(string[, level]) -- Compress a string with compression level.\n"
"\n"
"This is the simplest mechanism to perform zstd compression. The passed\n"
"string will be compressed and the result returned.\n"
"\n"
"An optional integer compression level can be specified.\n"
"\n"
"Compression is performed in a single operation. For larger inputs, it\n"
"is recommended to use the streaming compression interface to compress\n"
"in chunks");

static PyObject *pyzstd_compress(PyObject* self, PyObject *args) {
	const char* source;
	Py_ssize_t sourceSize;
	int compressionLevel = 3;
	size_t destSize;
	PyObject* output;
	char* dest;
	size_t zresult;

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTuple(args, "y#|i:compress", &source, &sourceSize, &compressionLevel)) {
#else
	if (!PyArg_ParseTuple(args, "s#|i:compress", &source, &sourceSize, &compressionLevel)) {
#endif
		return NULL;
	}

	if (compressionLevel < 1) {
		PyErr_SetString(PyExc_ValueError, "compressionlevel must be greater than 0");
		return NULL;
	}

	if (compressionLevel > ZSTD_maxCLevel()) {
		PyErr_Format(PyExc_ValueError, "compressionlevel must be less than %d",
				     ZSTD_maxCLevel() + 1);
		return NULL;
	}

	destSize = ZSTD_compressBound(sourceSize);

	output = PyBytes_FromStringAndSize(NULL, destSize);
	if (!output) {
		return NULL;
	}

	dest = PyBytes_AsString(output);

	Py_BEGIN_ALLOW_THREADS
	zresult = ZSTD_compress(dest, destSize, source, sourceSize, compressionLevel);
	Py_END_ALLOW_THREADS

	if (ZSTD_isError(zresult)) {
		PyErr_SetString(ZstdError, "compression error");
		Py_CLEAR(output);
	}
	else {
		Py_SIZE(output) = zresult;
	}

	return output;
}

typedef struct {
	PyObject_HEAD

	PyObject* writer;
	ZSTD_CStream* cstream;
	size_t insize;
	size_t outsize;
	int compressionLevel;
	void* dictData;
	size_t dictSize;
	CompressionParametersObject* cparams;
	int entered;
} pyzstd_compresswriter;

PyDoc_STRVAR(compresswriter__doc__,
"compresswriter(writer, compression_level=3, dict_data=None, compression_params=None)\n"
"Compress data to a writer object.\n"
"\n"
"Used as a context manager, the ``compresswriter`` is instantiated with an\n"
"object providing a ``write()`` method. As data is fed to the compresswriter\n"
"instance, compressed data is written to the writer.\n"
"\n"
"Callers can customize compression behavior by passing additional optional\n"
"arguments:\n"
"\n"
"compression_level\n"
"   Integer compression level.\n"
"dict_data\n"
"   Binary data holding a computed compression dictionary.\n"
"compression_params\n"
"   A ``CompressionParameters`` instance. If specified, this will overwrite\n"
"   the ``compression_level`` argument.\n"
);

static int pyzstd_compresswriter_init(pyzstd_compresswriter* self, PyObject* args, PyObject* kwargs) {
	static char* kwlist[] = {
		"writer",
		"compression_level",
		"dict_data",
		"compression_params",
		NULL
	};

	PyObject* writer;
	int compressionLevel = 3;
	const char* dictData;
	Py_ssize_t dictSize = 0;
	CompressionParametersObject* params = NULL;

	self->writer = NULL;
	self->cstream = NULL;
	self->dictData = NULL;
	self->dictSize = 0;
	self->cparams = NULL;
	self->entered = 0;

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iy#O!", kwlist,
#else
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|is#O!", kwlist,
#endif
		&writer, &compressionLevel, &dictData, &dictSize,
		&CompressionParametersType, &params)) {
		return -1;
	}

	if (compressionLevel < 1) {
		PyErr_SetString(PyExc_ValueError, "compressionlevel must be greater than 0");
		return -1;
	}

	if (compressionLevel > ZSTD_maxCLevel()) {
		PyErr_Format(PyExc_ValueError, "compressionlevel must be less than %d",
			ZSTD_maxCLevel() + 1);
		return -1;
	}

	if (!PyObject_HasAttrString(writer, "write")) {
		PyErr_SetString(PyExc_ValueError, "must pass an object with a write() method");
		/* TODO need DecRef on writer? */
		return -1;
	}

	self->writer = writer;
	Py_INCREF(writer);

	self->insize = ZSTD_CStreamInSize();
	self->outsize = ZSTD_CStreamOutSize();
	self->compressionLevel = compressionLevel;

	/* If we have dictionary data, copy it to an internal buffer.
	   We could potentially reference the PyObject here. Meh.
	*/
	if (dictSize) {
		self->dictData = malloc(dictSize);
		if (!self->dictData) {
			PyErr_NoMemory();
			return -1;
		}
		memcpy(self->dictData, dictData, dictSize);
		self->dictSize = dictSize;
	}

	if (params) {
		self->cparams = params;
		Py_INCREF(params);
	}

	return 0;
}

static void pyzstd_compresswriter_dealloc(pyzstd_compresswriter* self) {
	Py_XDECREF(self->writer);
	Py_XDECREF(self->cparams);

	if (self->dictData) {
		free(self->dictData);
		self->dictData = NULL;
	}

	PyObject_Del(self);
}

static PyObject* pyzstd_compresswriter_enter(pyzstd_compresswriter* self, PyObject* args) {
	size_t zresult;
	ZSTD_parameters zparams;

	if (self->entered) {
		PyErr_SetString(ZstdError, "cannot __enter__ multiple times");
		return NULL;
	}

	self->cstream = ZSTD_createCStream();
	if (!self->cstream) {
		PyErr_SetString(ZstdError, "cannot create CStream");
		return NULL;
	}

	if (self->cparams) {
		memset(&zparams, 0, sizeof(zparams));
		ztopy_compression_parameters(self->cparams, &zparams.cParams);
		zresult = ZSTD_initCStream_advanced(self->cstream,
			self->dictData, self->dictSize, zparams, 0);
	}
	else if (self->dictData) {
		zresult = ZSTD_initCStream_usingDict(self->cstream,
			self->dictData, self->dictSize, self->compressionLevel);
	}
	else {
		zresult = ZSTD_initCStream(self->cstream, self->compressionLevel);
	}

	if (ZSTD_isError(zresult)) {
		ZSTD_freeCStream(self->cstream);
		self->cstream = NULL;
		PyErr_SetString(ZstdError, "cannot init CStream");
		return NULL;
	}

	self->entered = 1;

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* pyzstd_compresswriter_compress(pyzstd_compresswriter* self, PyObject* args) {
	const char* source;
	Py_ssize_t sourceSize;
	size_t zresult = 0;
	ZSTD_inBuffer input;
	ZSTD_outBuffer output;

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTuple(args, "y#", &source, &sourceSize)) {
#else
	if (!PyArg_ParseTuple(args, "s#", &source, &sourceSize)) {
#endif
		return NULL;
	}

	if (!self->entered) {
		PyErr_SetString(ZstdError, "compress must be called from an active context manager");
		return NULL;
	}

	output.dst = malloc(self->outsize);
	if (!output.dst) {
		return PyErr_NoMemory();
	}
	output.size = self->outsize;
	output.pos = 0;

	/* Feed data from input argument */
	input.src = source;
	input.size = sourceSize;
	input.pos = 0;

	while ((ssize_t)input.pos < sourceSize) {
		Py_BEGIN_ALLOW_THREADS
		zresult = ZSTD_compressStream(self->cstream, &output, &input);
		Py_END_ALLOW_THREADS

		if (ZSTD_isError(zresult)) {
			free(output.dst);
			PyErr_SetString(ZstdError, "zstd decompress error");
			return NULL;
		}

		/* Write data from output buffer to writer. */
		if (output.pos) {
#if PY_MAJOR_VERSION >= 3
			PyObject_CallMethod(self->writer, "write", "y#",
#else
			PyObject_CallMethod(self->writer, "write", "s#",
#endif
				output.dst, output.pos);
		}
		output.pos = 0;
	}

	free(output.dst);

	Py_RETURN_NONE;
}

static PyObject* pyzstd_compresswriter_exit(pyzstd_compresswriter* self, PyObject* args) {
	size_t zresult;
	ZSTD_outBuffer output;
	self->entered = 0;

	if (self->cstream) {
		/* TODO only do this if no exception thrown in context manager */
		output.dst = malloc(self->outsize);
		/* TODO check malloc result */
		output.size = self->outsize;
		output.pos = 0;

		while (1) {
			zresult = ZSTD_endStream(self->cstream, &output);
			if (ZSTD_isError(zresult)) {
				/* TODO raise */
				return NULL;
			}

			if (output.pos) {
#if PY_MAJOR_VERSION >= 3
				PyObject_CallMethod(self->writer, "write", "y#",
#else
				PyObject_CallMethod(self->writer, "write", "s#",
#endif
					output.dst, output.pos);
			}

			if (!zresult) {
				break;
			}

			output.pos = 0;
		}

		free(output.dst);

		ZSTD_freeCStream(self->cstream);
		self->cstream = NULL;
	}

	Py_RETURN_NONE;
}

static PyMethodDef pyzstd_compresswriter_methods[] = {
	{ "__enter__", (PyCFunction)pyzstd_compresswriter_enter, METH_NOARGS,
	PyDoc_STR("Enter a compression context.") },
	{ "__exit__", (PyCFunction)pyzstd_compresswriter_exit, METH_VARARGS,
	PyDoc_STR("Exit a compression context.") },
	{ "compress", (PyCFunction)pyzstd_compresswriter_compress, METH_VARARGS,
	PyDoc_STR("compress a data chunk.") },
	{ NULL, NULL }
};

static PyTypeObject CompressWriterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"zstd.compresswriter",         /* tp_name */
	sizeof(pyzstd_compresswriter), /* tp_basicsize */
	0,                              /* tp_itemsize */
	(destructor)pyzstd_compresswriter_dealloc, /* tp_dealloc */
	0,                              /* tp_print */
	0,                              /* tp_getattr */
	0,                              /* tp_setattr */
	0,                              /* tp_compare */
	0,                              /* tp_repr */
	0,                              /* tp_as_number */
	0,                              /* tp_as_sequence */
	0,                              /* tp_as_mapping */
	0,                              /* tp_hash */
	0,                              /* tp_call */
	0,                              /* tp_str */
	0,                              /* tp_getattro */
	0,                              /* tp_setattro */
	0,                              /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	compresswriter__doc__,          /* tp_doc */
	0,                              /* tp_traverse */
	0,                              /* tp_clear */
	0,                              /* tp_richcompare */
	0,                              /* tp_weaklistoffset */
	0,                              /* tp_iter */
	0,                              /* tp_iternext */
	pyzstd_compresswriter_methods,  /* tp_methods */
	0,                              /* tp_members */
	0,                              /* tp_getset */
	0,                              /* tp_base */
	0,                              /* tp_dict */
	0,                              /* tp_descr_get */
	0,                              /* tp_descr_set */
	0,                              /* tp_dictoffset */
	(initproc)pyzstd_compresswriter_init, /* tp_init */
	0,                              /* tp_alloc */
	PyType_GenericNew,              /* tp_new */
};

/* Represents a decompresswriter instance. */
typedef struct {
	PyObject_HEAD

	PyObject* writer;
	ZSTD_DStream* dstream;
	void* dictData;
	size_t dictSize;
	size_t insize;
	size_t outsize;
	int entered;
} pyzstd_decompresswriter;

PyDoc_STRVAR(decompress_writer__doc__,
"decompresswriter(writer) -- Decompress data to a writer object.\n"
"\n"
"Used as a context manager, the ``decompresswriter`` is instantiated with an\n"
"object providing a ``write()`` method. As compressed data is fed to the\n"
"decompresswriter instance, uncompressed data is written to the writer.");

static int pyzstd_decompesswriter_init(pyzstd_decompresswriter* self, PyObject* args, PyObject* kwargs) {
	static char* kwlist[] = {
		"writer",
		"dict_data",
		NULL
	};

	PyObject* writer;
	const char* dictData;
	Py_ssize_t dictSize = 0;

	self->writer = NULL;
	self->dstream = NULL;
	self->dictData = NULL;
	self->dictSize = 0;
	self->entered = 0;

	/* TODO support for setting dstream parameters (currently just max window size */
#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|y#", kwlist,
#else
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|s#", kwlist,
#endif
		&writer, &dictData, &dictSize)) {
		return -1;
	}

	if (!PyObject_HasAttrString(writer, "write")) {
		PyErr_SetString(PyExc_ValueError, "must pass an object with a write() method");
		return -1;
	}

	self->writer = writer;
	Py_INCREF(writer);

	self->dstream = NULL;
	self->entered = 0;
	self->insize = ZSTD_DStreamInSize();
	self->outsize = ZSTD_DStreamOutSize();

	if (dictData) {
		self->dictData = malloc(dictSize);
		if (!self->dictData) {
			PyErr_NoMemory();
			return -1;
		}

		memcpy(self->dictData, dictData, dictSize);
		self->dictSize = dictSize;
	}

	return 0;
}

static void pyzstd_decompresswriter_dealloc(pyzstd_decompresswriter* self) {
	Py_XDECREF(self->writer);

	if (self->dictData) {
		free(self->dictData);
		self->dictData = NULL;
		self->dictSize = 0;
	}

	PyObject_Del(self);
}

static PyObject* pyzstd_decompresswriter_enter(pyzstd_decompresswriter* self, PyObject* args) {
	size_t zresult;

	if (self->entered) {
		PyErr_SetString(ZstdError, "cannot __enter__ multiple times");
		return NULL;
	}

	self->dstream = ZSTD_createDStream();
	if (!self->dstream) {
		PyErr_SetString(ZstdError, "cannot create DStream");
		return NULL;
	}

	if (self->dictData) {
		zresult = ZSTD_initDStream_usingDict(self->dstream, self->dictData, self->dictSize);
	}
	else {
		zresult = ZSTD_initDStream(self->dstream);
	}
	if (ZSTD_isError(zresult)) {
		ZSTD_freeDStream(self->dstream);
		self->dstream = NULL;
		PyErr_SetString(ZstdError, "cannot init DStream");
		return NULL;
	}

	self->entered = 1;

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* pyzstd_decompresswriter_decompress(pyzstd_decompresswriter* self, PyObject* args) {
	const char* source;
	Py_ssize_t sourceSize;
	size_t zresult = 0;
	ZSTD_inBuffer input;
	ZSTD_outBuffer output;

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTuple(args, "y#", &source, &sourceSize)) {
#else
	if (!PyArg_ParseTuple(args, "s#", &source, &sourceSize)) {
#endif
		return NULL;
	}

	if (!self->entered) {
		PyErr_SetString(ZstdError, "decompress must be called from an active context manager");
		return NULL;
	}

	output.dst = malloc(self->outsize);
	if (!output.dst) {
		return PyErr_NoMemory();
	}
	output.size = self->outsize;
	output.pos = 0;

	/* Feed data from input argument */
	input.src = source;
	input.size = sourceSize;
	input.pos = 0;

	while ((ssize_t)input.pos < sourceSize || zresult == 1) {
		Py_BEGIN_ALLOW_THREADS
		zresult = ZSTD_decompressStream(self->dstream, &output, &input);
		Py_END_ALLOW_THREADS

		if (ZSTD_isError(zresult)) {
			free(output.dst);
			PyErr_SetString(ZstdError, "zstd decompress error");
			return NULL;
		}

		/* Write data from output buffer to writer. */
		if (output.pos) {
#if PY_MAJOR_VERSION >= 3
			PyObject_CallMethod(self->writer, "write", "y#",
#else
			PyObject_CallMethod(self->writer, "write", "s#",
#endif
				output.dst, output.pos);
		}
		output.pos = 0;
	}

	free(output.dst);

	Py_RETURN_NONE;
}

static PyObject* pyzstd_decompresswriter_exit(pyzstd_decompresswriter* self, PyObject* args) {
	self->entered = 0;

	if (self->dstream) {
		ZSTD_freeDStream(self->dstream);
		self->dstream = NULL;
	}

	Py_RETURN_NONE;
}

static PyMethodDef pyzstd_decompresswriter_methods[] = {
	{ "__enter__", (PyCFunction)pyzstd_decompresswriter_enter, METH_NOARGS,
		PyDoc_STR("Enter a decompression context.") },
	{ "__exit__", (PyCFunction)pyzstd_decompresswriter_exit, METH_VARARGS,
		PyDoc_STR("Exit a decompression context.") },
	{ "decompress", (PyCFunction)pyzstd_decompresswriter_decompress, METH_VARARGS,
		PyDoc_STR("Decompress a data chunk.") },
	{ NULL, NULL }
};

static PyTypeObject DecompressWriterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"zstd.decompresswriter",         /* tp_name */
	sizeof(pyzstd_decompresswriter), /* tp_basicsize */
	0,                              /* tp_itemsize */
	(destructor)pyzstd_decompresswriter_dealloc, /* tp_dealloc */
	0,                              /* tp_print */
	0,                              /* tp_getattr */
	0,                              /* tp_setattr */
	0,                              /* tp_compare */
	0,                              /* tp_repr */
	0,                              /* tp_as_number */
	0,                              /* tp_as_sequence */
	0,                              /* tp_as_mapping */
	0,                              /* tp_hash */
	0,                              /* tp_call */
	0,                              /* tp_str */
	0,                              /* tp_getattro */
	0,                              /* tp_setattro */
	0,                              /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	decompress_writer__doc__,       /* tp_doc */
	0,                              /* tp_traverse */
	0,                              /* tp_clear */
	0,                              /* tp_richcompare */
	0,                              /* tp_weaklistoffset */
	0,                              /* tp_iter */
	0,                              /* tp_iternext */
	pyzstd_decompresswriter_methods, /* tp_methods */
	0,                              /* tp_members */
	0,                              /* tp_getset */
	0,                              /* tp_base */
	0,                              /* tp_dict */
	0,                              /* tp_descr_get */
	0,                              /* tp_descr_set */
	0,                              /* tp_dictoffset */
	(initproc)pyzstd_decompesswriter_init, /* tp_init */
	0,                              /* tp_alloc */
	PyType_GenericNew,              /* tp_new */
};

PyDoc_STRVAR(estimate_compress_context_size__doc__,
"estimate_compress_context_size(compression_parameters)\n"
"\n"
"Give the amount of memory allocated for a compression context given a\n"
"CompressionParameters instance");

static PyObject* pyzstd_estimate_compression_context_size(PyObject* self, PyObject* args) {
	CompressionParametersObject* params;
	ZSTD_compressionParameters zparams;
	PyObject* result;

	if (!PyArg_ParseTuple(args, "O!", &CompressionParametersType, &params)) {
		return NULL;
	}

	ztopy_compression_parameters(params, &zparams);
	result = PyLong_FromSize_t(ZSTD_estimateCCtxSize(zparams));
	Py_INCREF(result);
	return result;
}

PyDoc_STRVAR(get_compression_parameters__doc__,
"get_compression_parameters(compression_level[, source_size[, dict_size]])\n"
"\n"
"Obtains a ``CompressionParameters`` instance from a compression level and\n"
"optional input size and dictionary size");

static CompressionParametersObject* pyzstd_get_compression_parameters(PyObject* self, PyObject* args) {
	int compressionLevel;
	unsigned PY_LONG_LONG sourceSize = 0;
	Py_ssize_t dictSize = 0;
	ZSTD_compressionParameters params;
	CompressionParametersObject* result;

	if (!PyArg_ParseTuple(args, "i|Kn", &compressionLevel, &sourceSize, &dictSize)) {
		return NULL;
	}

	params = ZSTD_getCParams(compressionLevel, sourceSize, dictSize);

	result = PyObject_New(CompressionParametersObject, &CompressionParametersType);
	if (!result) {
		return NULL;
	}

	result->windowLog = params.windowLog;
	result->chainLog = params.chainLog;
	result->hashLog = params.hashLog;
	result->searchLog = params.searchLog;
	result->searchLength = params.searchLength;
	result->targetLength = params.targetLength;
	result->strategy = params.strategy;

	return result;
}

PyDoc_STRVAR(train_dictionary__doc__,
"train_dictionary(dict_size, samples)\n"
"\n"
"Train a dictionary from sample data.\n"
"\n"
"A compression dictionary of size ``dict_size`` will be created from the\n"
"iterable of samples provided by ``samples``.\n"
"\n"
"The raw dictionary content will be returned\n");

static PyObject* pyzstd_train_dictionary(PyObject* self, PyObject* args, PyObject* kwargs) {
	static char *kwlist[] = { "dict_size", "samples", "parameters", NULL };
	size_t capacity;
	PyObject* samples;
	Py_ssize_t samplesLen;
	PyObject* parameters = NULL;
	ZDICT_params_t zparams;
	Py_ssize_t sampleIndex;
	Py_ssize_t sampleSize;
	PyObject* sampleItem;
	size_t zresult;
	void* sampleBuffer;
	void* sampleOffset;
	size_t samplesSize = 0;
	size_t* sampleSizes;
	void* dict;

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "nO!|O!", kwlist,
		&capacity,
		&PyList_Type, &samples,
		(PyObject*)&DictParametersType, &parameters)) {
		return NULL;
	}

	/* Validate parameters first since it is easiest. */
	zparams.selectivityLevel = 0;
	zparams.compressionLevel = 0;
	zparams.notificationLevel = 0;
	zparams.dictID = 0;
	zparams.reserved[0] = 0;
	zparams.reserved[1] = 0;

	if (parameters) {
		/* TODO validate data ranges */
		zparams.selectivityLevel = PyLong_AsUnsignedLong(PyTuple_GetItem(parameters, 0));
		zparams.compressionLevel = PyLong_AsLong(PyTuple_GetItem(parameters, 1));
		zparams.notificationLevel = PyLong_AsUnsignedLong(PyTuple_GetItem(parameters, 2));
		zparams.dictID = PyLong_AsUnsignedLong(PyTuple_GetItem(parameters, 3));
	}

	/* Figure out the size of the raw samples */
	samplesLen = PyList_Size(samples);
	for (sampleIndex = 0; sampleIndex < samplesLen; sampleIndex++) {
		sampleItem = PyList_GetItem(samples, sampleIndex);
		if (!PyBytes_Check(sampleItem)) {
			PyErr_SetString(PyExc_ValueError, "samples must be bytes");
			/* TODO probably need to perform DECREF here */
			return NULL;
		}
		samplesSize += PyBytes_GET_SIZE(sampleItem);
	}

	/* Now that we know the total size of the raw simples, we can allocate
	   a buffer for the raw data */
	sampleBuffer = malloc(samplesSize);
	if (!sampleBuffer) {
		return PyErr_NoMemory();
	}
	sampleSizes = malloc(samplesLen * sizeof(size_t));
	if (!sampleSizes) {
		free(sampleBuffer);
		return PyErr_NoMemory();
	}

	sampleOffset = sampleBuffer;
	/* Now iterate again and assemble the samples in the buffer */
	for (sampleIndex = 0; sampleIndex < samplesLen; sampleIndex++) {
		sampleItem = PyList_GetItem(samples, sampleIndex);
		sampleSize = PyBytes_GET_SIZE(sampleItem);
		sampleSizes[sampleIndex] = sampleSize;
		memcpy(sampleOffset, PyBytes_AS_STRING(sampleItem), sampleSize);
		sampleOffset = (char*)sampleOffset + sampleSize;
	}

	dict = malloc(capacity);
	if (!dict) {
		free(sampleSizes);
		free(sampleBuffer);
		return PyErr_NoMemory();
	}

	zresult = ZDICT_trainFromBuffer_advanced(dict, capacity,
		sampleBuffer, sampleSizes, (unsigned int)samplesLen,
		zparams);
	if (ZDICT_isError(zresult)) {
		PyErr_Format(ZstdError, "Cannot train dict: %s", ZDICT_getErrorName(zresult));
		free(dict);
		free(sampleSizes);
		free(sampleBuffer);
		return NULL;
	}

	/* TODO reference counting foo */
	return PyBytes_FromStringAndSize((char*)dict, zresult);
}

PyDoc_STRVAR(dictionary_id__doc__,
"dictionary_id(dict)\n"
"\n"
"Obtain the dictionary ID value for a compression dictionary.");

static PyObject* pyzstd_dictionary_id(PyObject* self, PyObject* args) {
	char* source;
	Py_ssize_t sourceSize;
	unsigned dictID;

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTuple(args, "y#", &source, &sourceSize)) {
#else
	if (!PyArg_ParseTuple(args, "s#", &source, &sourceSize)) {
#endif
		PyErr_SetString(PyExc_ValueError, "must pass bytes data");
		return NULL;
	}

	dictID = ZDICT_getDictID((void*)source, sourceSize);
	return PyLong_FromLong(dictID);
}

static char zstd_doc[] = "Interface to zstandard";

static PyMethodDef zstd_methods[] = {
	{ "compress", (PyCFunction)pyzstd_compress, METH_VARARGS, compress__doc__ },
	{ "estimate_compression_context_size", (PyCFunction)pyzstd_estimate_compression_context_size, METH_VARARGS, estimate_compress_context_size__doc__ },
	{ "get_compression_parameters", (PyCFunction)pyzstd_get_compression_parameters, METH_VARARGS, get_compression_parameters__doc__ },
	{ "train_dictionary", (PyCFunction)pyzstd_train_dictionary, METH_VARARGS, train_dictionary__doc__ },
	{ "dictionary_id", (PyCFunction)pyzstd_dictionary_id, METH_VARARGS, dictionary_id__doc__ },
	{ NULL, NULL }
};

void zstd_module_init(PyObject* m) {
	PyObject* version;

	Py_TYPE(&CompressionParametersType) = &PyType_Type;
	if (PyType_Ready(&CompressionParametersType) < 0) {
		return;
	}

	Py_TYPE(&FrameParametersType) = &PyType_Type;
	if (PyType_Ready(&FrameParametersType) < 0) {
		return;
	}

	Py_TYPE(&DictParametersType) = &PyType_Type;
	if (PyType_Ready(&DictParametersType) < 0) {
		return;
	}

	Py_TYPE(&CompressWriterType) = &PyType_Type;
	if (PyType_Ready(&CompressWriterType) < 0) {
		return;
	}

	Py_TYPE(&DecompressWriterType) = &PyType_Type;
	if (PyType_Ready(&DecompressWriterType) < 0) {
		return;
	}

	ZstdError = PyErr_NewException("zstd.ZstdError", NULL, NULL);

	/* For now, the version is a simple tuple instead of a dedicated type. */
	version = PyTuple_New(3);
	PyTuple_SetItem(version, 0, PyLong_FromLong(ZSTD_VERSION_MAJOR));
	PyTuple_SetItem(version, 1, PyLong_FromLong(ZSTD_VERSION_MINOR));
	PyTuple_SetItem(version, 2, PyLong_FromLong(ZSTD_VERSION_RELEASE));
	Py_IncRef(version);
	PyModule_AddObject(m, "ZSTD_VERSION", version);

	Py_IncRef((PyObject*)&CompressionParametersType);
	PyModule_AddObject(m, "CompressionParameters", (PyObject*)&CompressionParametersType);

	Py_IncRef((PyObject*)&FrameParametersType);
	PyModule_AddObject(m, "FrameParameters", (PyObject*)&FrameParametersType);

	Py_IncRef((PyObject*)&DictParametersType);
	PyModule_AddObject(m, "DictParameters", (PyObject*)&DictParametersType);

	Py_INCREF(&CompressWriterType);
	PyModule_AddObject(m, "compresswriter", (PyObject*)&CompressWriterType);

	Py_INCREF(&DecompressWriterType);
	PyModule_AddObject(m, "decompresswriter", (PyObject*)&DecompressWriterType);

	PyModule_AddIntConstant(m, "MAX_COMPRESSION_LEVEL", ZSTD_maxCLevel());
	PyModule_AddIntConstant(m, "COMPRESSION_RECOMMENDED_INPUT_SIZE",
		(long)ZSTD_CStreamInSize());
	PyModule_AddIntConstant(m, "COMPRESSION_RECOMMENDED_OUTPUT_SIZE",
		(long)ZSTD_CStreamOutSize());
	PyModule_AddIntConstant(m, "DECOMPRESSION_RECOMMENDED_INPUT_SIZE",
		(long)ZSTD_DStreamInSize());
	PyModule_AddIntConstant(m, "DECOMPRESSION_RECOMMENDED_OUTPUT_SIZE",
		(long)ZSTD_DStreamOutSize());

	PyModule_AddIntConstant(m, "MAGIC_NUMBER", ZSTD_MAGICNUMBER);
	PyModule_AddIntConstant(m, "WINDOWLOG_MIN", ZSTD_WINDOWLOG_MIN);
	PyModule_AddIntConstant(m, "WINDOWLOG_MAX", ZSTD_WINDOWLOG_MAX);
	PyModule_AddIntConstant(m, "CHAINLOG_MIN", ZSTD_CHAINLOG_MIN);
	PyModule_AddIntConstant(m, "CHAINLOG_MAX", ZSTD_CHAINLOG_MAX);
	PyModule_AddIntConstant(m, "HASHLOG_MIN", ZSTD_HASHLOG_MIN);
	PyModule_AddIntConstant(m, "HASHLOG_MAX", ZSTD_HASHLOG_MAX);
	PyModule_AddIntConstant(m, "HASHLOG3_MAX", ZSTD_HASHLOG3_MAX);
	PyModule_AddIntConstant(m, "SEARCHLOG_MIN", ZSTD_SEARCHLOG_MIN);
	PyModule_AddIntConstant(m, "SEARCHLOG_MAX", ZSTD_SEARCHLOG_MAX);
	PyModule_AddIntConstant(m, "SEARCHLENGTH_MIN", ZSTD_SEARCHLENGTH_MIN);
	PyModule_AddIntConstant(m, "SEARCHLENGTH_MAX", ZSTD_SEARCHLENGTH_MAX);
	PyModule_AddIntConstant(m, "TARGETLENGTH_MIN", ZSTD_TARGETLENGTH_MIN);
	PyModule_AddIntConstant(m, "TARGETLENGTH_MAX", ZSTD_TARGETLENGTH_MAX);

	PyModule_AddIntConstant(m, "STRATEGY_FAST", ZSTD_fast);
	PyModule_AddIntConstant(m, "STRATEGY_DFAST", ZSTD_dfast);
	PyModule_AddIntConstant(m, "STRATEGY_GREEDY", ZSTD_greedy);
	PyModule_AddIntConstant(m, "STRATEGY_LAZY", ZSTD_lazy);
	PyModule_AddIntConstant(m, "STRATEGY_LAZY2", ZSTD_lazy2);
	PyModule_AddIntConstant(m, "STRATEGY_BTLAZY2", ZSTD_btlazy2);
	PyModule_AddIntConstant(m, "STRATEGY_BTOPT", ZSTD_btopt);
}

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef zstd_module = {
	PyModuleDef_HEAD_INIT,
	"zstd",
	zstd_doc,
	-1,
	zstd_methods
};

PyMODINIT_FUNC PyInit_zstd(void)
#else
PyMODINIT_FUNC initzstd(void)
#endif
{
	PyObject *m;

#if PY_MAJOR_VERSION >= 3
	m = PyModule_Create(&zstd_module);
#else
	m = Py_InitModule3("zstd", zstd_methods, zstd_doc);
#endif

	if (!m) {
		return NULL;
	}

	zstd_module_init(m);

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}
