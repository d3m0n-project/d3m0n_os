Usage
=====

.. _installation:

Installation
------------
.. _prebuilt:

Pre-Build image file
____________________

To install d3m0n OS, first download lastest pre-built flashable `img file <https://github.com/d3m0n-project/d3m0n_os/releases>`_:

Install `Balena Etcher <https://etcher.balena.io/>`_ and flash your microSD card, insert it into your d3m0n device and boot!


.. _custom:

Custom image file
____________________

Clone this repo using ``git clone https://github.com/d3m0n-project/d3m0n_os.git`` then move into

.. code-block:: console

   $ chmod +x *.sh
   $ sudo ./d3m0n.sh

Then choose build 

Creating recipes
----------------

To retrieve a list of random ingredients,
you can use the ``lumache.get_random_ingredients()`` function:

.. autofunction:: lumache.get_random_ingredients

The ``kind`` parameter should be either ``"meat"``, ``"fish"``,
or ``"veggies"``. Otherwise, :py:func:`lumache.get_random_ingredients`
will raise an exception.

.. autoexception:: lumache.InvalidKindError

For example:

>>> import lumache
>>> lumache.get_random_ingredients()
['shells', 'gorgonzola', 'parsley']

