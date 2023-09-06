Usage
=====

.. _installation:

Installation
------------

To use d3m0n_os, first install it using pip:

.. code-block:: console

   (.venv) $ pip install d3m0n_os

Creating recipes
----------------

To retrieve a list of random ingredients,
you can use the ``d3m0n_os.get_random_ingredients()`` function:

.. autofunction:: d3m0n_os.get_random_ingredients

The ``kind`` parameter should be either ``"meat"``, ``"fish"``,
or ``"veggies"``. Otherwise, :py:func:`d3m0n_os.get_random_ingredients`
will raise an exception.

.. autoexception:: d3m0n_os.InvalidKindError

For example:

>>> import d3m0n_os
>>> d3m0n_os.get_random_ingredients()
['shells', 'gorgonzola', 'parsley']

